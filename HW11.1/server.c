#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/ioctl.h>
#define MAX 30
#define N 256
#define LOCALHOST "127.0.0.1"
#define BUFFER 1024

int main(int argc, char **argv) {
    int sockfd, connfd, maxfdp1, on = 1, rc;
    char buff[BUFFER];
    struct sockaddr_in servaddr, cliaddr;
    char* message = "Hello Client";
    fd_set rset; 
    pid_t pid;
    if (argc != 2) {
        printf("Input: %s <port>\n", argv[0]);
        return 0;
    }

    int PORT = atoi(argv[1]);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    };
    rc = setsockopt(sockfd, SOL_SOCKET,  SO_REUSEADDR,
                   (char *)&on, sizeof(on));
    if (rc < 0){
        perror("setsockopt() failed");
        close(sockfd);
        exit(-1);
    }

    rc = ioctl(sockfd, FIONBIO, (char *)&on);
    if (rc < 0){
      perror("ioctl() failed");
      close(sockfd);
      exit(-1);
   }


    memset(&servaddr, 0, sizeof(servaddr));
    // memset(&cliaddr, 0, sizeof(cliaddr));
      
    servaddr.sin_family    = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = inet_addr(LOCALHOST);
    servaddr.sin_port = htons(PORT);
    if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if ((listen(sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }
    socklen_t cli_addr_size = sizeof(cliaddr);
    FD_ZERO(&rset); 
	FD_SET(sockfd, &rset);
	maxfdp1 = sockfd;

    printf("Server started on %s:%d\n", LOCALHOST, PORT);
    while(1) {

        rc = select(maxfdp1+1, &rset, NULL, NULL, NULL);
        if (rc < 0) {
            perror("  select() failed");
            break;
        }

        if (rc == 0) {
            printf("  select() timed out.  End program.\n");
            break;
        }


        if(FD_ISSET(sockfd, &rset)) {
            if ((connfd = accept(sockfd, (struct sockaddr*)&cliaddr, &cli_addr_size)) < 0 ) {
                perror("server accept failed...\n");
                exit(EXIT_FAILURE);
            }
            printf("Connection accepted from %s:%d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
            if ((pid = fork()) == 0) {
                close(sockfd);
                while(1) {
                    rc = read(connfd, buff, sizeof(buff));
                    if (rc < 0 ) {
                        if (errno != EWOULDBLOCK) {
                            perror("  recv() failed");
                            exit(EXIT_FAILURE);
                        }
                    }
                    if (rc == 0) {
                        printf("  Connection closed\n");
                        return 0;
                    }
                
                    printf("Data received: %s\n" , buff);
                    write(connfd, (const char*)message, sizeof(buff));
                }
            }
        }
    }
    close(connfd);
    close(sockfd);
    return 0;
    
}
