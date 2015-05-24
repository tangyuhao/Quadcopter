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
#include "fifo.h"
int fifo_create_read_nonblock(char * fifo_name)
{
	int fifo_fd_id;
	if(access(fifo_name, 0) == 0)  unlink(fifo_name);	//if exist ,delete
	int fifo_create = mkfifo(fifo_name,0666);//create a fifo for command reading and writing
	if (fifo_create < 0) 
	{
		perror("mkfifo error!");
		return -1;
	}
	else
	{
		fifo_fd_id = open(fifo_name,O_NONBLOCK|O_RDWR);
		if (fifo_fd_id == -1)
		{
			perror("fifo open error");
			return -2;
		}
		else 
			return fifo_fd_id;
	}
}
int main()
{
	if ((data_fifo_fd = fifo_create_read(DATA_FIFO_NAME)) < 0)
		{
			perror("error create data_fifo ");
			return DATA_FIFO_CREATE_ERROR;
		}
}
