#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/shm.h>


/*Define cmd_type*/
#define CONTROL_TYPE	0x01
#define CHANNEL_TYPE	0x02

/*Define different control commands*/
#define LEVEL		0x01
#define TAKEOFF		0x02
#define ARM			0x04
#define DISARM		0x08

#define MYPORT  8008
#define QUEUE   20
#define BUFFER_SIZE 1024
/*Creat a structure to store data info*/
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
		float rollspeed,pitchspeed,yawspeed;
		float hud_alt, hud_climb,hud_groundspeed;
};

/*Create a struct to transport the data*/
struct receive_struct
{
	unsigned char head[4];
	int len;
	struct status_struct info;
};
struct receive_struct recv_data;

/*Create a structure to hold channel command value*/
struct rc_struct
{
	unsigned char chan[4];
	unsigned char mode;
};


/*Create a union to store different types of data*/
union data_struct{
	struct rc_struct rc;
	unsigned char control;

};

/*Create a struct to manage the command*/
struct cmd_struct
{
	unsigned char head[2];
	unsigned char type;
	unsigned char len;
	unsigned char control;
};
struct cmd_struct cmd;
ssize_t wrap_recv(int sock, void *buf, size_t len, int flags)
{
	ssize_t recvd=0, res;
	int tmperr;	
	int timeout_cnt = 0;
	while (recvd<len){
		res=recv(sock, buf+recvd, len-recvd, flags);
		if (res<0){
			tmperr=errno;
			if (tmperr==EINTR || tmperr==EAGAIN || tmperr==EWOULDBLOCK)
				continue;
			errno=tmperr;
			return res;
		}
		else if(res == 0)
		{
			usleep(10000);
			printf("TimeoutCnt=%d\n",timeout_cnt);
			if(timeout_cnt++ >= 300)
			{
				printf("recv timeout\n");
				return 0;
			}
		}
		recvd+=res;
	}
	return recvd;
}
static void except_recv(int sock, void *buf, size_t len, int flags)
{
	ssize_t ret;
	ret = wrap_recv(sock, buf, len, flags);
	if(ret < 0)
	{
		perror("wrap_recv error");
		//*state_flag = SOCK_ERROR;
		//exit(-1);
	}
	else if(ret == 0)
	{
		printf("sock timeout\n");
		//*state_flag = SOCK_TIMEOUT;
	}
	return;
}

int main()
{
    ///定义sockfd
    int server_sockfd = socket(AF_INET,SOCK_STREAM, 0);

    ///定义sockaddr_in
    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(MYPORT);
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    ///bind，成功返回0，出错返回-1
    if(bind(server_sockfd,(struct sockaddr *)&server_sockaddr,sizeof(server_sockaddr))==-1)
    {
        perror("bind");
        exit(1);
    }

    ///listen，成功返回0，出错返回-1
    if(listen(server_sockfd,QUEUE) == -1)
    {
        perror("listen");
        exit(1);
    }

    ///客户端套接字
    char recvbuffer[BUFFER_SIZE];
	char sendbuffer[BUFFER_SIZE];
    struct sockaddr_in client_addr;
    socklen_t length = sizeof(client_addr);

    ///成功返回非负描述字，出错返回-1
    int conn = accept(server_sockfd, (struct sockaddr*)&client_addr, &length);
    if(conn<0)
    {
        perror("connect");
        exit(1);
    }
/*
	recv_data.head[0] = 0xff;
	recv_data.head[1] = 0xaa;
	recv_data.head[2] = 0xbb;
	recv_data.head[3] = 0xcc;
*/
	cmd.type = CONTROL_TYPE;
	cmd.len = 0x000f;
	cmd.control = LEVEL;
	int k; 
	char buffer[BUFFER_SIZE];

    while(1)
    {
		printf("len = %d  \n",cmd.len);
		sleep(1);
		k=0;
        //memset(buffer,0,sizeof(buffer));
       // int len = recv(conn, buffer, sizeof(buffer),0);
		printf("%d    \n",k);
		except_recv(server_sockfd, &recv_data, sizeof(recv_data), 0);
		printf("received data:recv_data = %s\n",buffer);
		//printf("received data:len = %d\n",recv_data.len);
		//k=send(conn, &cmd, sizeof(cmd),0); ///发送
		printf("%d    \n",k);
      //  fputs(buffer, stdout);
        //send(conn, buffer, len, 0);
    }
    close(conn);
    close(server_sockfd);
    return 0;
}
