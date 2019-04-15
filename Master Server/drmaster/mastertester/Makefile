
CC=gcc
#CC=egcs
#CFLAGS=-Wall

all: tester
drmaster: tester.o
	$(CC) $(CFLAGS) -o tester tester.o 
tester.o: tester.c
	$(CC) $(CFLAGS) -c tester.c
clean:
	rm *.o;rm tester
