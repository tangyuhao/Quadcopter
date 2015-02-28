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

//header files except system's ones
#include "mavproxy.h"
#include "fifo.h"

int main()
{
	int fifo_create;
	pid_t pid_fork;
	extern int cmd_fifo_fd;
	extern int data_fifo_fd;
	struct status_struct status;
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
