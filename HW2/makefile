CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 

all: resolver

resolver: main.o lib.o 
	${CC} main.o lib.o -o resolver

main.o: main.c
	${CC} ${CFLAGS} main.c

lib.o: lib.c
	${CC} ${CFLAGS} lib.c


clean:
	rm -f *.o *~