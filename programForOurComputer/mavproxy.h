#ifndef __MAVPROXY_H__
#define __MAVPROXY_H__

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


void write2mavproxy(char *cmd_buf);
int write2mavproxy_rc(int channel,int chan_value);
int write2mavproxy_mode(int mav_mode);
int msleep(int delay_time);
int write2mavproxy_status(struct status_struct* sta);

#endif
