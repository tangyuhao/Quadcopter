vpath % .h socket:mavproxy:fifo:control_flag:serial
CFLAGS = -g -I./socket -I./mavproxy -I./fifo -I./control_flag -I./serial 
CC = gcc
RM = -rm -rf
main_mavproxy: main_mavproxy.o mavproxy.o fifo.o socket.o serial_func.o
	${CC} $^ -O2 -o $@  
%.o:%.c
	${CC}  -c $(CFLAGS) $< 
.PHONY:clean
clean:
	${RM} *.o
	


