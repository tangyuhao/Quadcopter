#ifndef __CONTROL_H__
#define __CONTROL_H__
#define BEAGLEBONE 0 // whether the program is running on the Beaglebone or on the PC
#define DEBUG 1      // switch for debug mode
#define CHAN_DEBUG 1 // channel output debug mode
#define WP_DEBUG 0   // retained for the waypoint debug mode
#define ST_DEBUG 1   // status_flag debug mode
#define STATUS_DEBUG 0 // status from the mavproxy 
#define SAFE_DOWN 0    // ensure chan3 not too low when over X meters
#define SAFE_CHAN12 1  // whether to turn on the SAFEMODE FOR CHAN1 AND CHAN2
#define SAFE_HENCE 0   // whether to turn on the SAFEHENCE
#define NEED_GPS 0     // refer to main_mavproxy.c
#define TEST 1         // if you are testing the aeroplane, you can set this macro definition
#endif
