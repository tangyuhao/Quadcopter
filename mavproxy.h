#ifndef __MAVPROXY_H__
#define __MAVPROXY_H__

#define DEBUG 0
#if DEBUG
    #define DEBUG_PRINTF(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define DEBUG_PRINTF(format, ...) 
#endif

#define CHAN_DEBUG 0
#if CHAN_DEBUG
    #define CH_PRINTF(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define CH_PRINTF(format, ...) 
#endif

#define WP_DEBUG 0
#if WP_DEBUG
    #define WP_PRINTF(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define WP_PRINTF(format, ...) 
#endif

#define ST_DEBUG 0
#if ST_DEBUG
    #define ST_PRINTF(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define ST_PRINTF(format, ...) 
#endif

#define CLEAR(x) memset((x), 0, sizeof(x))

/*Define the channel*/
#define ROLL		1
#define PITCH		2
#define THROTTL		3
#define YAW		4


/*Define the flying mode*/
#define STABILIZE 1
#define LAND 2
#define LOITER 3
#define ALT_HOLD 4
#define AUTO 5

/*Define State Flag*/
#define RECV_HEADER		0
#define RECV_CONTROL		1
#define RECV_CHANNEL		2
#define RECV_MODE	   	3
#define SOCK_TIMEOUT		-1
#define SOCK_ERROR		-2
#define MAV_TIMEOUT		-3
#define MAV_ERROR		-4
#define ARM_ERROR		-5

/*Define the results*/
#define UNLINK_SUCCEED 0
#define UNLINK_FAILED -1
#define SIGKILL_SUCCEED 0
#define SIGKILL_FAILED -1
#define CLOSE_SUCCEED 0
#define CLOSE_FAILED -1

/*Define the error*/
#define CMD_FIFO_CREATE_ERROR -1
#define DATA_FIFO_CREATE_ERROR -2
#define CMD_FIFO_REMOVE_ERROR -3
#define DATA_FIFO_REMOVE_ERROR -4
#define THRESHOLD_TAKEOFF 150
#define THRESHOLD_FLYING 250

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

/*Create a struct to manage the command*/
struct cmd_struct
{
	unsigned char head[2];
	char level[4],arm[4],takeoff[4];
	int chan[4];
	char mode[4];
};
struct cmd_struct cmd;

/*Define Functions*/
void write2mavproxy(char *cmd_buf);
int write2mavproxy_rc(int channel,int chan_value);
int write2mavproxy_mode(int mav_mode);
int msleep(int delay_time);
int write2mavproxy_status(struct status_struct* sta);

#endif
