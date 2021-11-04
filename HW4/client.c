#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include "lib.h"
#define MAXCHAR 100
#define BUFFER 1024

void die(char *s)
{
	perror(s);
	exit(1);
}

int main(int argc, char **argv) {
    char ip[MAXCHAR];
    int sockfd = 0;
    struct sockaddr_in serv_addr;
    char input[BUFFER], request[BUFFER], response[BUFFER];
    socklen_t addr_size;
    if (argc < 3) {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        return -1;
    }
    
    strcpy(ip, argv[1]);
    int port = atoi(argv[2]);

    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        die("socket creation failed");
    }

    memset(&serv_addr, '\0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr(ip);

    while(1) {
        fgets(input, BUFFER, stdin);
        strtok(input, "\n");
        strcpy(request, input);
        if (strcmp(request, "\n") == 0) {
            die("Exit");
        }
        //send the message
		if (sendto(sockfd, request, BUFFER, 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
			die("sendto()");
		}

        // Try to receive some data, this is a blocking call
        if (recvfrom(sockfd, response, BUFFER, 0, (struct sockaddr *) &serv_addr, &addr_size) < 0) {
            die("recvfrom()");
        }

        printf("%s\n", response);
    }

    close(sockfd);
}
