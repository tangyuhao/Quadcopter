#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>


/*Define cmd_type*/
#define CONTROL_TYPE	0x01
#define CHANNEL_TYPE	0x02

/*Define different control commands*/
#define LEVEL		0x01
#define TAKEOFF		0x02
#define ARM			0x04
#define DISARM		0x08

#define MYPORT  8888
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
		double rollspeed,pitchspeed,yawspeed;
		double hud_alt, hud_climb,hud_groundspeed;
};

/*Create a struct to transport the data*/
struct send_struct
{
	unsigned char head[2];
	struct status_struct status;
};
struct send_struct send_data;

/*Create a structure to hold channel command value*/
struct rc_struct
{
	short chan[4];
	char mode;
};


/*Create a union to store different types of data*/
union data_struct{
	struct rc_struct rc;
	struct status_struct status;
	char control;

};

/*Create a struct to manage the command*/
struct cmd_struct
{
	unsigned char head[2];
	char type;
	short len;
	char control;
};
struct cmd_struct cmd;
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

	cmd.head[0] = 0xff;
	cmd.head[1] = 0xaa;
	cmd.type = CONTROL_TYPE;
	cmd.len = 0x000f;
	cmd.control = LEVEL;
	int k; 

    while(1)
    {
		printf("len = %d  \n",cmd.len);
		sleep(1);
		k=0;
        //memset(buffer,0,sizeof(buffer));
       // int len = recv(conn, buffer, sizeof(buffer),0);
		printf("%d    \n",k);
		k=send(conn, &cmd, sizeof(cmd),0); ///发送
		printf("%d    \n",k);
      //  fputs(buffer, stdout);
        //send(conn, buffer, len, 0);
    }
    close(conn);
    close(server_sockfd);
    return 0;
}
