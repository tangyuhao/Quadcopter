#ifndef __MAVPROXY_H__
#define __MAVPROXY_H__

#include <control.h>
#if DEBUG
    #define DEBUG_PRINTF(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define DEBUG_PRINTF(format, ...) 
#endif


#if CHAN_DEBUG
    #define CH_PRINTF(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define CH_PRINTF(format, ...) 
#endif


#if WP_DEBUG
    #define WP_PRINTF(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define WP_PRINTF(format, ...) 
#endif


#if ST_DEBUG
    #define ST_PRINTF(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define ST_PRINTF(format, ...) 
#endif

#if STATUS_DEBUG
    #define STATUS_PRINTF(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define STATUS_PRINTF(format, ...) 
#endif

#if NEED_GPS
    #define GPS_TEST sta->satellites_visible<4
#else
    #define GPS_TEST 0 
#endif

#define CLEAR(x) memset((x), 0, sizeof(x))

/*Define the constant*/
#define PI	3.1415926

/*Define the safe parameters*/
#define SAFE_DOWN_HEIGHT	3.5

/*Define the channel*/
#define ROLL		0
#define PITCH		1
#define THROTTL		2
#define YAW		3


/*Define the flying mode*/
#define STABILIZE 0x01
#define LAND 0x02
#define LOITER 0x03
#define ALT_HOLD 0x04
#define AUTO 0x05

/*Define different control commands*/
#define LEVEL		0x01
#define TAKEOFF		0x02
#define ARM			0x04
#define DISARM		0x08


/*Define cmd_type*/
#define CONTROL_TYPE	0x01
#define CHANNEL_TYPE	0x02
#define STATUS_TYPE     0X03
#define AUTOFLY_TYPE	0x04
#define LAND_TYPE	0x05
#define LEVEL_TYPE	0x06
#define DISARM_TYPE	0x07


/*Define State Flag*/
#define RECV_HEADER		0
#define RECV_PARAM      1
#define RECV_CONTROL	2
#define SEND_STATUS		3
#define RECV_CHANNEL	4
#define AUTO_TAKEOFF    5
#define RECV_LAND       6
#define RECV_LEVEL      7
#define RECV_DISARM	    8
#define SOCK_TIMEOUT	-1
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

/*Define the error for auto take off*/
#define FAIL_ALOFT -1
#define FAIL_GROUND -2
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
	float roll_degree, pitch_degree,yaw_degree;
};

/*Creat a structure for sending the status*/
struct send_status
{
	unsigned char head[3];
	unsigned char flag;
	int len;
	struct status_struct info;
}status;

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
	union data_struct data;
};
struct cmd_struct cmd;

/*Define Functions*/
short autoTakeoff(float height,unsigned short step, unsigned short throttle_max, float fail_threshold);
void write2mavproxy(char *cmd_buf);
int write2mavproxy_rc(int channel,int chan_value);
int write2mavproxy_mode(int mav_mode);
int msleep(int delay_time);
int write2mavproxy_status(struct status_struct* sta);


#endif
