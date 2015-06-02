#ifndef __FIFO_H__
#define __FIFO_H__

#define CMD_FIFO_NAME "cmd_fifo"
#define DATA_FIFO_NAME "data_fifo"	

int fifo_create_read(char * fifo_name);
int close_rm_fifo(int fd,char * file_name);


#endif
