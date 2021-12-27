CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 

all: client server

client: client.o lib.o
	${CC} client.o lib.o -o client

server: server.o network.o lib.o
	${CC} server.o network.o lib.o -o server -pthread

server.o: server.c
	${CC} ${CFLAGS} server.c -pthread

client.o: client.c
	${CC} ${CFLAGS} client.c

network.o: network.c
	${CC} ${CFLAGS} network.c

lib.o: lib.c
	${CC} ${CFLAGS} lib.c

clean:
	rm -f *.o *~