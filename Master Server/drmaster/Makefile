
CC=gcc
#CC=egcs
#CFLAGS=-Wall

all: drmaster
drmaster: drmaster.o
	$(CC) $(CFLAGS) -o drmaster drmaster.o 
#netstuff.o: netstuff.c netstuff.h
#	$(CC) $(CFLAGS) -c netstuff.c
drmaster.o: drmaster.c
	$(CC) $(CFLAGS) -c drmaster.c
clean:
	rm *.o;rm drmaster
install:
	cp drmaster /usr/local/bin
