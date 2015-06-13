#ifndef __SERIAL_FUNC_H__
#define __SERIAL_FUNC_H__

#define FILE_OPEN_ERROR		"open file failed"
#define BAUD_RATE_ERROR		"set baudrate failed"
#define GET_ATTR_ERROR		"get attribute failed"
#define SET_ATTR_ERROR		"set attribute failed"



int open_port(const char *dev);
void close_port(int fd);
int serial_init(int fd);
int set_baudrate(int fd, int baud);
int set_dataformat(int fd, int databits, int parity, int stopbits);
int open_and_init_serial(char *serial_dev);


#endif
