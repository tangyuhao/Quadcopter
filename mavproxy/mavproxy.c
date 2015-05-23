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
#include "mavproxy.h"
#include "fifo.h"
int cmd_fifo_fd;
int data_fifo_fd;

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
	STATUS_PRINTF("********************************status info***********************************\n");
	STATUS_PRINTF("motor1:%d\tmotor2:%d\tmotor3:%d\tmotor4:%d\n",(*sta).motor_speed1,(*sta).motor_speed2,
		(*sta).motor_speed3,(*sta).motor_speed4);
	STATUS_PRINTF("ch1:%d\tch2:%d\tch3:%d\tch4:%d\tch5:%d\tch6:%d\tch7:%d\tch8:%d\n",
		(*sta).chan1,(*sta).chan2,(*sta).chan3,(*sta).chan4,(*sta).chan5,(*sta).chan6,(*sta).chan7,(*sta).chan8);
	STATUS_PRINTF("voltage_remain:%dmV\tcurrent_remain:%dmA\tbattery_remain:%d%%\n",
		(*sta).vol_remain,(*sta).cur_remain,(*sta).bat_remain);
	STATUS_PRINTF("alttitude:%fm\tclimb_speed:%fm/s\tground_speed:%fm/s\n",
			(*sta).hud_alt,(*sta).hud_climb,(*sta).hud_groundspeed);

	STATUS_PRINTF("******************************************************************************\n");
	}
	return 0;
}


