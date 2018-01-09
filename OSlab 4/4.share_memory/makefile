all : sender receiver
.PHONY : clean


sender : sender.o 
	cc -pthread -o sender sender.o

receiver : receiver.o 
	cc -pthread -o receiver receiver.o 

 
sender.o : share_memory.h

receiver.o : share_memory.h

clean : 
	rm	receiver
	rm	sender 
	rm	*.o