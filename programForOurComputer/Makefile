vpath % .h ../include :include
CFLAGS = -I../include -I./include
RM = -rm -rf

main_mavproxy: main_mavproxy.o mavproxy.o fifo.o
	${CC} -o $@ $^
%.o:%.c
	${CC} -c $(CFLAGS) $<
.PHONY:clean
clean:
	${RM} *.o
	


