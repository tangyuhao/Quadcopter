#include<stdio.h>
#include<linux/serial.h>
#include<fcntl.h>
#include<unistd.h>
#include<linux/termios.h>/*POSIX terminal control definitions*/
#include<errno.h>
#include"serial_func.h"

static struct termios origin_attrs;
static struct termios old_attrs;
static struct termios Opt;
/*
 * Func Name: void open_port(int fd)-- open serial port 0
 * Switch GPIO into UART mode
 * Returns the rile descriptor on success or -1 on error
 */
int open_port(const char *dev)
{
/*set GPIO configure*/
//	char *device = "/dev/ttyS0";
	int fd; /*File descriptor for the port*/
	int ret;
	char arg[300] = "/gpio/gpio_setup_ttys0.sh";
	system(arg);

/*open options: O_NONBLOCK enables non-block mode*/
//	fd = open(device, O_RDWR|O_NOCTTY|O_NDELAY);
//	fd = open(dev, O_RDWR|O_NONBLOCK| O_NOCTTY);
	fd = open(dev, O_RDWR|O_NONBLOCK);
	perror("open_port: open /dev/ttyS0");

/*use isatty to check if device is a terminal*/
	ret = isatty(STDIN_FILENO);
	if(!ret)
	{
		perror("standard input error!");
		close(fd);
		return -1;
	}
	

	return(fd);
}

/*Func Name:void close_port(int fd)-- close serial port 0
 *Retrieve the old port attributes and close the device
 * */
void close_port(int fd)
{
	int ret;
	ret = tcsetattr(fd, TCSANOW, &old_attrs);
	if(ret)
	{
		perror("GET_ATTR_ERROR");
	}
	close(fd);
		
}

/*Func Name: int serial_init(int fd)-- initialize the seiral port 0
 * set serial port 0's baud rate, data length etc...
 */
int serial_init(int fd)
{
	int Baud_Rate = B9600;
	
	tcgetattr(fd, &Opt);
 	/*Enalbe the receiver and set local mode*/
	
	Opt.c_cflag = (CS8 | CLOCAL | CREAD);
	Opt.c_iflag = (IGNPAR | IXON | IXOFF + IXANY);
	Opt.c_lflag = 0;
	Opt.c_oflag = 0;
	Opt.c_cc[VTIME] = 30;
	Opt.c_cc[VMIN] = 5;
	tcflush(fd, TCIOFLUSH);
	cfsetispeed(&Opt, Baud_Rate);
	cfsetospeed(&Opt, Baud_Rate);
	if(tcsetattr(fd, TCSANOW, &Opt) != 0)
	{
		perror("set attribute failed");
		return -1;
	}

	return fd;

}

/*Func Name: int set_baudrate(int fd, int baud) 
 *Paremeters:
  fd : file handle of opening serial port 0
  baud: serial port0's speed
 *Description:modify serial port's baud rate
 *return: fd(normal),-1(error)
 * */
int set_baudrate(int fd, int baud)
{
	char buffer[1000];
	int Baud_Rate;
	if(tcgetattr(fd, &Opt))
	{
		perror("GET_ATTR_ERROR");
		return -1;
	}
	switch(baud)
	{
		case 115200:	Baud_Rate = B115200;break;
		case 57600:	Baud_Rate = B57600;break;
		case 38400:	Baud_Rate = B38400;break;
		case 19200:	Baud_Rate = B19200;break;
		case 9600:	Baud_Rate = B9600;break;
		case 4800:	Baud_Rate = B4800;break;
		case 2400:	Baud_Rate = B2400;break;
		case 1200:	Baud_Rate = B1200;break;
		case 600:	Baud_Rate = B600;break;
		case 300:	Baud_Rate = B300;break;
		default:	perror("Wrong Baud rate");return -1;
	}
	
	tcflush(fd, TCIOFLUSH);//clear all present I/O data
	cfsetispeed(&Opt, Baud_Rate);
	cfsetospeed(&Opt, Baud_Rate);
	tcsetattr(fd, TCSANOW, &Opt);
	
	return fd;
}

/*Func Name:int set_dataformat(int fd, databits,int parity, int stopbits)
 *Parameter:
  fd: file handle of opened device
  databits: can be chose from 5-8
  parity: 'n'/'N'--no check, 'o'/'O'--odd, 'e'/'E'--even
  stopbtis: 1 or 2 bits are available
 * Descrition:set databits parity & stopbits
 * return:-1--get attr error, -2 -- set databits error, -3 -- set parity error, -4 -- set stop bits error, fd -- normal 
 * */
int set_dataformat(int fd, int databits, int parity, int stopbits)
{
	volatile char buffer[1000];
	if(tcgetattr(fd, &Opt))
	{
		perror("GET_ATTR_ERROR");
		return -1;
	}
	Opt.c_cflag &= ~CSIZE;
	switch(databits)
	{
		case 5: Opt.c_cflag |= CS5;
			break;
		case 6: Opt.c_cflag |= CS6;
			break;
		case 7: Opt.c_cflag |= CS7;
			break;
		case 8: Opt.c_cflag |= CS8;
			break;
		default:
			perror("Unsupported data size");
			return -2;
	}
	switch(parity)
	{	
		case 'n':	//no parity check
		case 'N':
			Opt.c_cflag &= ~PARENB;
			Opt.c_iflag &= ~INPCK;
			break;
		case 'o':	//odd check
		case 'O':
			Opt.c_cflag |= (PARODD | PARENB);
			Opt.c_iflag |= (INPCK | ISTRIP);
			break;
		case 'e':	//even check
		case 'E':
			Opt.c_cflag |= PARENB;
			Opt.c_iflag &= ~PARODD;
			Opt.c_iflag |= (INPCK | ISTRIP);
			break;
		default:
			perror("Unsupported parity");
			return -3;
	switch(stopbits)
	{
		case 1:		//1 stop bit
			Opt.c_cflag &= ~CSTOPB;
			break;
		case 2:
			Opt.c_cflag |= CSTOPB;
			break;
		default:
			perror("Unsupported stop bits");
			return -4;
	}
	tcflush(fd, TCIFLUSH);
	Opt.c_cc[VTIME] = 50;
	Opt.c_cc[VMIN] = 1;
	if(tcsetattr(fd, TCSANOW, &Opt)!=0)
	{
		perror("SET_ATTR_ERROR");
		return -1;
	}
	return 0;
}

int store_termios(int fd)
{
	if(tcgetattr(fd, &origin_attrs))
	{
		perror("GET_ATTR_ERROR");
		return -1;
	}
	return 0;
}
int recover_termios(int fd)
{
	tcflush(fd, TCIOFLUSH);
	if(tcsetattr(fd, TCSANOW, &origin_attrs))
	{
		perror("SET_ATTR_ERROR");
		return -1;
	}
	tcflush(fd, TCIOFLUSH);
	return 0;
}
}

int open_and_init_serial(char *serial_dev)
{
	int ret, fd;
	fd = open_port(serial_dev);
	ret = serial_init(fd);
	ret = set_baudrate(fd, 57600);
	printf("set_baudrate:%d\n", ret);
	set_dataformat(fd, 8, 'o', 1);

	return fd;
}
