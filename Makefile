vpath % .h socket:mavproxy:fifo
CFLAGS = -I./socket -I./mavproxy -I./fifo 
RM = -rm -rf

main_mavproxy: main_mavproxy.o mavproxy.o fifo.o socket.o
	${CC} -o $@ $^
%.o:%.c
	${CC} -c $(CFLAGS) $<
.PHONY:clean
clean:
	${RM} *.o
	


