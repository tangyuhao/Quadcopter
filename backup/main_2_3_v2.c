#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <math.h>


#define CMD_FIFO_NAME "cmd_fifo"
#define DATA_FIFO_NAME "data_fifo"
#define STABILIZE 1
#define LAND 2
#define LOITER 3
#define ALT_HOLD 4
#define AUTO 5
#define UNLINK_SUCCEED 0
#define UNLINK_FAILED -1
#define SIGKILL_SUCCEED 0
#define SIGKILL_FAILED -1
#define CLOSE_SUCCEED 0
#define CLOSE_FAILED -1
#define CMD_FIFO_CREATE_ERROR -1
#define DATA_FIFO_CREATE_ERROR -2
#define CMD_FIFO_REMOVE_ERROR -3
#define DATA_FIFO_REMOVE_ERROR -4
#define THRESHOLD_TAKEOFF 150
#define THRESHOLD_FLYING 250

int cmd_fifo_fd;
int data_fifo_fd;
struct status_struct
{
		int arm;
		int xacc,yacc,zacc;
		int motor_speed1,motor_speed2,motor_speed3,motor_speed4;
		int heading_north;
		int lat,lon;
		int eph,satellites_visible;
		int chan1, chan2, chan3, chan4, chan5, chan6, chan7, chan8;
		int vol_remain, cur_remain, bat_remain;
		double rollspeed,pitchspeed,yawspeed;
		double hud_alt, hud_climb,hud_groundspeed;
}status;

void write2mavproxy(char *cmd_buf)
{
    write(cmd_fifo_fd, cmd_buf, strlen(cmd_buf));
    write(cmd_fifo_fd, "\n", 1);	
}

int write2mavproxy_rc(int channel,int chan_value)
{
	char buffer[20];
	if (channel > 4 || channel <1 ||chan_value >2000 || chan_value <1000)
	{
		printf("invalid rc setting!\n");
		return 0;
	}
	sprintf(buffer,"rc %d %d",channel,chan_value);
	write2mavproxy(buffer);
	return 1;
}

int write2mavproxy_mode(int mav_mode)
{
	int returned_value = 1;
	switch (mav_mode)
	{
		case STABILIZE:write2mavproxy("mode stabilize"); break;
		case LAND:write2mavproxy("mode land");break;
		case LOITER:write2mavproxy("mode loiter");break;
		case ALT_HOLD:write2mavproxy("mode alt_hold");break;
		case AUTO:write2mavproxy("mode auto");break;
		default:printf("invalid mode!\n");returned_value = 0;break;
	}
	return returned_value;
}

int msleep(int delay_time)
{
	if (delay_time > 100000 || delay_time < 0)
	{
		printf("time is longer than 10 seconds or smaller than 0!/n");
		return 0;
	}
	usleep(delay_time * 1000);
	return 1;
}
int fifo_create_read(char * fifo_name)
{
	int fifo_fd_id;
	if(access(fifo_name, 0) == 0)  unlink(fifo_name);	//if exist ,delete
	int fifo_create = mkfifo(fifo_name,0666);//create a fifo for command reading and writing
	if (fifo_create < 0) 
	{
		perror("mkfifo error!");
		return -1;
	}
	else
	{
		fifo_fd_id = open(fifo_name,O_RDWR);
		if (fifo_fd_id == -1)
		{
			perror("fifo open error");
			return -2;
		}
		else 
			return fifo_fd_id;
	}
}
int close_rm_fifo(int fd,char * file_name)
{
	char buffer1[50],buffer2[50];
	sprintf(buffer1,"close %s failed",file_name);
	sprintf(buffer2,"unlink %s failed",file_name);
	if(close(fd) == CLOSE_SUCCEED) 
		printf("close %s complete!\n",file_name);
	else 
		perror(buffer1);
	if(unlink(file_name) != UNLINK_SUCCEED) 
	{
		perror(buffer2);
		return -1;
	}
	else
		printf("unlink %s complete!\n",file_name);

	return 0;
}
int write2mavproxy_status(struct status_struct* sta)
{
	/*read status data from Mavproxy*/
	int status_len = sizeof(*sta);
	write2mavproxy("status");
	int	nread = 0;
	nread = read(data_fifo_fd, sta, status_len);
	if(nread == -1 && errno != EAGAIN )
	{	
		perror("read data error");
		return -1;
	}
	else if(nread ==0)
	{
		printf("read data length error");
		return -2;
	}
	else if(nread == status_len)
	{
	printf("********************************status info***********************************\n");
	printf("motor1:%d\tmotor2:%d\tmotor3:%d\tmotor4:%d\n",(*sta).motor_speed1,(*sta).motor_speed2,
		(*sta).motor_speed3,(*sta).motor_speed4);
	printf("ch1:%d\tch2:%d\tch3:%d\tch4:%d\tch5:%d\tch6:%d\tch7:%d\tch8:%d\n",
		(*sta).chan1,(*sta).chan2,(*sta).chan3,(*sta).chan4,(*sta).chan5,(*sta).chan6,(*sta).chan7,(*sta).chan8);
	printf("voltage_remain:%dmV\tcurrent_remain:%dmA\tbattery_remain:%d%%\n",
		(*sta).vol_remain,(*sta).cur_remain,(*sta).bat_remain);
	printf("alttitude:%fm\tclimb_speed:%fm/s\tground_speed:%fm/s\n",
			(*sta).hud_alt,(*sta).hud_climb,(*sta).hud_groundspeed);

	printf("******************************************************************************\n");
	}
	return 0;
}

int main()
{
	int fifo_create;
	pid_t pid_fork;
//	printf("status length is%d\n",status_len);
	if ((cmd_fifo_fd = fifo_create_read(CMD_FIFO_NAME)) < 0)
	{
		perror("error create cmd_fifo ");
		return CMD_FIFO_CREATE_ERROR;
	}
	if ((data_fifo_fd = fifo_create_read(DATA_FIFO_NAME)) < 0)
	{
		perror("error create data_fifo ");
		return DATA_FIFO_CREATE_ERROR;
	}
	if ((pid_fork = fork()) == 0)
	{
		dup2(cmd_fifo_fd, STDIN_FILENO);
		execl("/usr/local/bin/mavproxy_pro.py","mavproxy_pro.py","--master=/dev/ttyO1","--baudrate=57600",NULL);
	}
	else
	{
//*******************start arming*************************//
		sleep(10);
		write2mavproxy_mode(STABILIZE);
		sleep(1);
		write2mavproxy_rc(1,1516);
		msleep(100);
		write2mavproxy_rc(2,1511);
		msleep(100);
		write2mavproxy_rc(3,1100);
		msleep(100);
		write2mavproxy_rc(4,1508);
		msleep(100);
		write2mavproxy("param set RC2_TRIM 1511");
		sleep(1);
		write2mavproxy("level");
		sleep(5);
		write2mavproxy_status(&status);
		sleep(1);
		write2mavproxy_rc(1,1516);
		msleep(100);
		write2mavproxy_rc(2,1511);
		msleep(100);
		write2mavproxy_rc(3,1100);
		msleep(100);
		write2mavproxy_rc(4,1508);
		msleep(100);
		write2mavproxy_rc(4,1900);
		sleep(1);
		write2mavproxy_rc(4,1900);
		sleep(1);
		write2mavproxy_rc(4,1900);
		sleep(1);
		write2mavproxy_rc(4,1508);
		msleep(100);
		int i;
		int motor12,motor13,motor14,motor23,motor24,motor34;
		
		char motor_right = 0;
		for (i=1;i<9;i++)
		{
			write2mavproxy_rc(3,1140+40*i);
			sleep(1);
			write2mavproxy_status(&status);
			motor12 = abs(status.motor_speed1 - status.motor_speed2);
			motor13 = abs(status.motor_speed1 - status.motor_speed3);
			motor14 = abs(status.motor_speed1 - status.motor_speed4);
			motor23 = abs(status.motor_speed2 - status.motor_speed3);
			motor24 = abs(status.motor_speed2 - status.motor_speed4);
			motor34 = abs(status.motor_speed3 - status.motor_speed4);
			if (motor12 < THRESHOLD_TAKEOFF && motor13 < THRESHOLD_TAKEOFF && 
				motor14 < THRESHOLD_TAKEOFF && motor23 < THRESHOLD_TAKEOFF 
				&& motor24 < THRESHOLD_TAKEOFF && motor34 < THRESHOLD_TAKEOFF)
				printf("right speed\t%d\t%d\t%d\t%d\t%d\t%d\n",motor12,motor13,motor14,motor23,motor24,motor34);
			else 
			{
				write2mavproxy_mode(LAND);
				printf("mode land\n");
				sleep(10);
				break;
			}
			msleep(100);
		}
		sleep(1);
		write2mavproxy_mode(ALT_HOLD);//if gps signal is good,we change it to LOITER
		
		for (i=0;i<9;i++)
		{
			write2mavproxy_mode(ALT_HOLD);
			msleep(500);
			write2mavproxy_status(&status);
			motor12 = abs(status.motor_speed1 - status.motor_speed2);
			motor13 = abs(status.motor_speed1 - status.motor_speed3);
			motor14 = abs(status.motor_speed1 - status.motor_speed4);
			motor23 = abs(status.motor_speed2 - status.motor_speed3);
			motor24 = abs(status.motor_speed2 - status.motor_speed4);
			motor34 = abs(status.motor_speed3 - status.motor_speed4);
			if (motor12 < THRESHOLD_FLYING && motor13 < THRESHOLD_FLYING 
				&& motor14 < THRESHOLD_FLYING && motor23 < THRESHOLD_FLYING 
				&& motor24 < THRESHOLD_FLYING && motor34 < THRESHOLD_FLYING)
				printf("right speed\t%d\t%d\t%d\t%d\t%d\t%d\n",motor12,motor13,motor14,motor23,motor24,motor34);
			else 
			{
				write2mavproxy_mode(LAND);
				printf("mode land\n");
				sleep(10);
				break;
			}
			msleep(100);
		}
		write2mavproxy_mode(LAND);
		sleep(10);
//*******************remove fifo before end***********************//
		close_rm_fifo(data_fifo_fd,DATA_FIFO_NAME);
		close_rm_fifo(cmd_fifo_fd,CMD_FIFO_NAME);
        if (kill(pid_fork, SIGKILL) != SIGKILL_SUCCEED) 
			perror("kill cmd_fork_pid failed");
		else
			printf("kill %d complete!\n ",pid_fork);
		pause();
		return (0);
		

	
	}
	
	
	
}
