#ifndef __SERIAL_H__
#define __SERIAL_H__

#define SERIAL_FALSE  -1
#define SERIAL_TRUE   0
void set_speed(int fd, int speed);
int set_Parity(int fd,int databits,int stopbits,int parity);
#endif
