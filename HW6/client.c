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

int main(int argc, char **argv) {
    char ip[MAXCHAR];
    // int n;
    int sockfd = 0;
    struct sockaddr_in serv_addr;
    char request[BUFFER], response[BUFFER];
    if (argc < 3) {
        printf("Input: %s <ip> <port>\n", argv[0]);
        return 0;
    }
    
    strcpy(ip, argv[1]);
    int port = atoi(argv[2]);

    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr(ip);
    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }
    printf("Username: \n");
    while(1) {
        int flag = 0;
        flag = scanf("%[^\n]", request);
        if (flag == 0 )  {
            printf("Client exit ...\n");
            break;
        }
        getchar();
        write(sockfd, request, sizeof(request));
        read(sockfd, response, sizeof(response));
        printf("%s\n", response);
        
    }

    close(sockfd);
}
