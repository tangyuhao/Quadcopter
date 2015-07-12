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

#include "mavproxy.h"
#include "fifo.h"

/* Func Name: int fifo_create_read(char * fifo_name)
 * Description:  create fifo and make it for reading and writing
 * Parameters: 
 *		fifo_name : the name of the fifo including the whole path
 * return: int
 * */
int fifo_create_read(char * fifo_name)
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
		fifo_fd_id = open(fifo_name,O_RDWR);
		if (fifo_fd_id == -1)
		{
			perror("fifo open error");
			return -2;
		}
		else 
			return fifo_fd_id;
	}
}
/* Func Name: int close_rm_fifo(int fd,char * file_name)
 * Description:  close fifo
 * Parameters:
 *		fd : the fd number of the fifo created before
 *		fifo_name : the name of the fifo including the whole path
 * return: int
 * */
int close_rm_fifo(int fd,char * file_name)
{
	char buffer1[50],buffer2[50];
	sprintf(buffer1,"close %s failed",file_name);
	sprintf(buffer2,"unlink %s failed",file_name);
	if(close(fd) == CLOSE_SUCCEED) 
		printf("close %s complete!\n",file_name);
	else 
		perror(buffer1);
	if(unlink(file_name) != UNLINK_SUCCEED) 
	{
		perror(buffer2);
		return -1;
	}
	else
		printf("unlink %s complete!\n",file_name);

	return 0;
}

