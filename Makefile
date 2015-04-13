vpath % .h socket:mavproxy:fifo
CFLAGS = -g -I./socket -I./mavproxy -I./fifo 
CC = gcc
RM = -rm -rf
main_mavproxy: main_mavproxy.o mavproxy.o fifo.o socket.o
	${CC} $^ -o $@  
%.o:%.c
	${CC}  -c $(CFLAGS) $< 
.PHONY:clean
clean:
	${RM} *.o
	


