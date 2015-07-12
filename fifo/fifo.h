#ifndef __FIFO_H__
#define __FIFO_H__

#define CMD_FIFO_NAME "cmd_fifo" // used for conveying command of mavproxy
#define DATA_FIFO_NAME "data_fifo"	// used for get status infomation from mavproxy_forPC.py \
										or mavproxy_forDebian.py
int cmd_fifo_fd;
int data_fifo_fd;
int fifo_create_read(char * fifo_name);
int close_rm_fifo(int fd,char * file_name);


#endif
