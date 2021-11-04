CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 

all: resolver

resolver: main.o lib.o network.o
	${CC} main.o lib.o network.o -o resolver

main.o: main.c
	${CC} ${CFLAGS} main.c

lib.o: lib.c
	${CC} ${CFLAGS} lib.c

network.o: network.c
	${CC} ${CFLAGS} network.c

clean:
	rm -f *.o *~