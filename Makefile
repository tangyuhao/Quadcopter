#add the path for .h files
vpath % .h socket:mavproxy:fifo:control_flag:serial        				
CFLAGS = -g -I./socket -I./mavproxy -I./fifo -I./control_flag -I./serial #you need to add all the paths of the including .c files 
CC = gcc        #use gcc as the compiler
RM = -rm -rf	#the option to delete the .o files
main_mavproxy: main_mavproxy.o mavproxy.o fifo.o socket.o serial_func.o
	${CC} $^ -O2 -o $@  # make the executable file from the .o files
%.o:%.c
	${CC}  -c $(CFLAGS) $<    #make .o files from .c files
.PHONY: 				
	clean           #use "make clean" to clean the .o files
clean:                          
	${RM} *.o
	


