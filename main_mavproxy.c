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
#include "socket.h"

pid_t pid_fork;
int client_sockfd_recv;
int client_sockfd_send;
char buf[BUFSIZ];
char *ip_addr_recv;
char *ip_addr_send;
int auto_flag;
long nbytes;
int throttl_val;
int yaw_val;
int pitch_val;
int roll_val;
int delay_cnt;
int arm_state;
/*Interupt Handle*/
static void sigint_handler(int signo)
{
	kill(pid_fork, SIGKILL);
	wait(NULL);
	printf("\n[%d]Caught SIGINT!\n", getpid());
	printf("[%d]MAVPROXY PROC killed\n", pid_fork);
	system("rm -rf *.fifo *.tlog*");
	exit(EXIT_SUCCESS);
}

/*Create a function to dispose the exception of wrap_recv function*/
static void except_recv(int sock, void *buf, size_t len, int flags, int *state_flag)
{
	ssize_t ret;
	ret = wrap_recv(sock, buf, len, flags);
	if(ret < 0)
	{
		perror("wrap_recv error");
		*state_flag = SOCK_ERROR;
		//exit(-1);
	}
	else if(ret == 0)
	{
		printf("sock timeout\n");
		*state_flag = SOCK_TIMEOUT;
	}
	return;
}

int main(int argc, char *argv[])
{
	int i;
	int motor12,motor13,motor14,motor23,motor24,motor34;
	int chan[4];
	char motor_right = 0;
	int fifo_create;
	pid_t pid_fork;
	pid_t pid_statusSend;
	extern int cmd_fifo_fd;
	extern int data_fifo_fd;
	status.head[0] = 0xff;
	status.head[1] = 0xaa;
	status.head[2] = 0xbb;
	status.head[3] = 0xcc;
	status.len = sizeof(status.info);
	
/*write a test help*/
	if(argc == 2)
	{
		DEBUG_PRINTF("argv[1]:%s\n", argv[1]);
		if(strcmp(argv[1], "-h")==0 || \
		strcmp(argv[1],"--help") == 0)
		{
			printf("This is a test program of receiving channel value 				from GCS and send it to Mavproxy\n");
			printf("You need to start our VirtualGCS as server at first\n");
			printf("Usage: (sudo) ./main [xxx.xxx.xxx.xxx:port_recv] [port_send]\ne.g. 127.0.0.1:8000 8008");
			return 0;
		}	
		else
		{
			printf("wrong instrument. Just rerun the program or use '-h','--help' for help\n");
			return 0;
		}
	}
	else if(argc == 3)
	{
		if((strlen(argv[1]) <= strlen("127.168.100.100:xxxx\0")) && 
			(strlen(argv[2]) == strlen("xxxx\0")))
		{
			ip_addr_recv = (char *)malloc(sizeof(argv[1]));
			ip_addr_recv = argv[1];
			mksock_send_ip(argv[1],argv[2]);
			ip_addr_send = argv[2];
			DEBUG_PRINTF("The recv port is %s\nThe send port is %s\n",argv[1],argv[2]);
		}
		else
		{
			printf("a wrong instrument. Just rerun the program or use '-h','--help' for help\n");
			return 0;
		}
	}
	else if (argc == 1)
	{
		char ar1[20]="127.0.0.1:8000";
		char ar2[20]="8008";
		ip_addr_recv = (char *)malloc(sizeof(ar1));
		ip_addr_recv = ar1;
		mksock_send_ip(ar1,ar2);
		ip_addr_send = ar2;
		DEBUG_PRINTF("The recv port is %s\nThe send port is %s\n",ar1,ar2);

	}
	else
	{
		printf("Parameter input error, please using '-h' or '--help' for help\n");
		return 0;
	}
	/*Regist signal interupt*/
	if(signal(SIGINT, sigint_handler)== SIG_ERR)
	{
		printf("Cannot handle SIGINT!\n");
		exit(EXIT_FAILURE);
	}

	/*Create a socket*/

    if((client_sockfd_recv = wrap_client(ip_addr_recv))<0)
	{
		perror("client_sockfd_recv");
		return -1;
	}
	else
		printf("Beaglebone connected to Ground Station(recv), fd = %d\n",client_sockfd_recv);
	msleep(200);	

	if((client_sockfd_send = wrap_client(ip_addr_send))<0)
		{
			perror("client_sockfd_send");
			return -1;
		}
	else
		printf("Beaglebone connected to Ground Station(send), fd = %d\n",client_sockfd_send);
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
		#if BEAGLEBONE 
		dup2(cmd_fifo_fd, STDIN_FILENO);
		execl("/usr/local/bin/mavproxy_pro.py","mavproxy_pro.py","--master=/dev/ttyO1","--baudrate=57600",NULL);
		#else 
		dup2(cmd_fifo_fd, STDIN_FILENO);
	//	execl("/usr/local/bin/mavproxy_pro_forPC.py","mavproxy_pro_forPC.py","--master=/dev/ttyUSB0","--baudrate=57600",NULL);
		execl("/usr/local/bin/mavproxy_pro_forPC.py","mavproxy_pro_forPC.py","--master=/dev/ttyUSB0","--baudrate=57600",NULL);
		#endif
	}

	if ((pid_statusSend = fork()) == 0)
	{
		int ret,status_len;
		status_len = sizeof(status);
		while(1)
	{
		msleep(400);
		//read status data from Mavproxy
		write2mavproxy_status(&status.info);
		//Send status data to GCS
		ret = wrap_send(client_sockfd_send, &status, status_len, 0);
		if(ret == -1)
		{
			perror("wrap_send error");
			return -1;
		}
	}

	}

	else
	{
		int nread, i;
		int state_flag = RECV_HEADER;
		char cmd_buf[100],control_flag;
		int cnt = 0;
		//arm_state = ARM_IDLE;
		throttl_val = 0;
		yaw_val = 0;
		pitch_val = 0;
		roll_val = 0;
//*******************start arming*************************
		sleep(10);
		//Initialze the flying mode to STABILIZE
		write2mavproxy_mode(STABILIZE);
		sleep(1);
		//set params and rc channels
		write2mavproxy_rc(1,1520);
		msleep(100);
		write2mavproxy_rc(2,1510);
		msleep(100);
		write2mavproxy_rc(3,1100);
		msleep(100);
		write2mavproxy_rc(4,1510);
		msleep(100);
		write2mavproxy("param set RC1_TRIM 1520");
		msleep(50);
		write2mavproxy("param set RC2_TRIM 1510");
		msleep(50);
		write2mavproxy("param set RC3_TRIM 1100");
		msleep(50);
		write2mavproxy("param set RC4_TRIM 1510");
		msleep(50);
		write2mavproxy("level");
		sleep(5);
		//while for sending and receiving
		int ret,status_len;
		status_len = sizeof(status);
		DEBUG_PRINTF("SIZE of status is:%d\n",status_len);
		DEBUG_PRINTF("SIZE of status.info is:%d\n",sizeof(status.info));

		while(1)
		{
			printf("state_flag=%d\n", state_flag);
			//[State 0] Receive header
			if(state_flag == RECV_HEADER)
			{
				CLEAR(&cmd);
				except_recv(client_sockfd_recv, cmd.head, sizeof(cmd.head), 0, &state_flag);
				if(state_flag == SOCK_TIMEOUT)
					continue;
				if(cmd.head[0] == 0xff && cmd.head[1] == 0xaa)
				{
					state_flag = RECV_PARAM;
				}
				//If cmd.head[1] receives 0xFF
				 else if(cmd.head[1] == 0xff)
				{
					cmd.head[0] = cmd.head[1];
					except_recv(client_sockfd_recv, &cmd.head[1], sizeof(cmd.head[1]), 0, &state_flag);
					if(state_flag == SOCK_TIMEOUT)
						continue;
					if(cmd.head[1] == 0xaa)
						state_flag = RECV_PARAM;
				}
				DEBUG_PRINTF("cmd.head[0]=%x cmd.head[1]=%x\n", cmd.head[0], cmd.head[1]);
			}
		
			//[State 1] Receive data parameter
			else if(state_flag == RECV_PARAM)
			{
				except_recv(client_sockfd_recv, &cmd.type, sizeof(cmd.type), 0, &state_flag);
				if(state_flag == SOCK_TIMEOUT)
					continue;
				DEBUG_PRINTF("cmd.type=%d\n", cmd.type);
				switch(cmd.type)
				{
					case CONTROL_TYPE:
						state_flag = RECV_CONTROL; break;
					case CHANNEL_TYPE:
						state_flag = RECV_CHANNEL; break;
					default:
						state_flag = RECV_HEADER;
				}
				//Receive data length
				except_recv(client_sockfd_recv, &cmd.len, sizeof(cmd.len), 0, &state_flag);
				if(state_flag == SOCK_TIMEOUT)
					continue;
				DEBUG_PRINTF("cmd.len:%d\n", cmd.len);
			}
			//[State 2] Receive channel values
			else if(state_flag == RECV_CHANNEL)
			{
				if(cmd.len != sizeof(cmd.data.rc))
				{
					DEBUG_PRINTF("Received length are %d, but expected to be %d\n", cmd.len, (int)sizeof(cmd.data.rc));
					state_flag = RECV_HEADER;
					continue;
				}
				
				cnt++;
				CH_PRINTF("cnt:%d\n",cnt);

				except_recv(client_sockfd_recv, &cmd.data.rc, cmd.len, 0, &state_flag);
				chan[0] = cmd.data.rc.chan[0]*10;
				chan[1] = cmd.data.rc.chan[1]*10;
				chan[2] = cmd.data.rc.chan[2]*10;
				chan[3] = cmd.data.rc.chan[3]*10;
				if(state_flag == SOCK_TIMEOUT)
					continue;
				//Print Received Command
				CH_PRINTF("***This is COMMAND test from C***\n");
				CH_PRINTF("chan1:%d, chan2:%d, chan3:%d, chan4:%d\n",
					chan[0],chan[1],chan[2],chan[3]);
				//CH_PRINTF("ARM status: %d\n", cmd.data.rc.arm);

				//write new channel values to Mavproxy
				//Write the throttle channel, we expect this channel could be updated periodically
				if(throttl_val != chan[THROTTL] && chan[THROTTL] != -1)
					throttl_val = chan[THROTTL];
				write2mavproxy_rc(THROTTL+1,throttl_val);
				//Write the mode channel; if mode channel switched, we expect we could use command to control it
				if(cmd.data.rc.mode != -1)
				{
					switch(cmd.data.rc.mode)
					{
						case STABILIZE: write2mavproxy_mode(STABILIZE);			break;
						case LOITER:	write2mavproxy_mode(LOITER);			break;
						case ALT_HOLD:	write2mavproxy_mode(ALT_HOLD);			break;
						case LAND:		write2mavproxy_mode(LAND);				break;
						case AUTO:		
										//write2mavproxy_mode(AUTO);	
										break;
						default:
										DEBUG_PRINTF("mode_flag error: VAL not found in <STABILIZE|LOITER|ALT_HOLD|AUTO|LAND>");		
										write2mavproxy_mode(LAND);
										state_flag = MAV_ERROR;	
										continue;
					}
				}
				//Write other channels if they changed
				for(i = 0; i <= 3; i++)
				{
					if(i != THROTTL)
						write2mavproxy_rc(i+1,chan[i]);
				}
			
				state_flag = RECV_HEADER;
			}
			//[State 3] Receive control commands
			else if (state_flag == RECV_CONTROL)  
			{

				
				if (cmd.len != sizeof(cmd.data.control))
				{
					DEBUG_PRINTF("Received length are %d, but expected to be %d\n", cmd.len, (int)sizeof(cmd.data.control));
					state_flag = RECV_HEADER;
					continue;
				}	
				except_recv(client_sockfd_recv, &cmd.data.control, cmd.len, 0, &state_flag);
				if(state_flag == SOCK_TIMEOUT)
					continue;		
				control_flag = cmd.data.control;	
				switch(control_flag)
					{
						case LEVEL:
							break;
						case ARM  :
							break;
						case DISARM:
							break;
						case TAKEOFF:
							break;
						default:
							continue;
					}
				state_flag = RECV_HEADER;
			}
		   	//[State 4] Receive error commands
        	else if(state_flag == SOCK_TIMEOUT || state_flag == SOCK_ERROR)
			{
				DEBUG_PRINTF("Entering Timeout State\n");
				sleep(1);
				write2mavproxy("mode LAND");
				close_rm_fifo(data_fifo_fd,DATA_FIFO_NAME);
				close_rm_fifo(cmd_fifo_fd,CMD_FIFO_NAME);
				if (kill(pid_fork, SIGKILL) != SIGKILL_SUCCEED) 
					perror("kill cmd_fork_pid failed");
				else
					printf("kill %d complete!\n ",pid_fork);
				if (kill(pid_statusSend, SIGKILL) != SIGKILL_SUCCEED) 
					perror("kill cmd_statusSend_pid failed");
				else
					printf("kill %d complete!\n ",pid_statusSend);
				pause();
				kill(getpid(), SIGINT);
			}
		}
	}
}
/*		write2mavproxy("level");
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
			motor12 = abs(status.info.motor_speed1 - status.info.motor_speed2);
			motor13 = abs(status.info.motor_speed1 - status.info.motor_speed3);
			motor14 = abs(status.info.motor_speed1 - status.info.motor_speed4);
			motor23 = abs(status.info.motor_speed2 - status.info.motor_speed3);
			motor24 = abs(status.info.motor_speed2 - status.info.motor_speed4);
			motor34 = abs(status.info.motor_speed3 - status.info.motor_speed4);
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
			motor12 = abs(status.info.motor_speed1 - status.info.motor_speed2);
			motor13 = abs(status.info.motor_speed1 - status.info.motor_speed3);
			motor14 = abs(status.info.motor_speed1 - status.info.motor_speed4);
			motor23 = abs(status.info.motor_speed2 - status.info.motor_speed3);
			motor24 = abs(status.info.motor_speed2 - status.info.motor_speed4);
			motor34 = abs(status.info.motor_speed3 - status.info.motor_speed4);
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
*/
