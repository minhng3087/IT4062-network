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
#include <fcntl.h>
#define MAX 30
#define N 256
#define LOCALHOST "127.0.0.1"
#define BUFFER 1024
#define TRUE 1
#define FALSE 0

int main(int argc, char ** argv) {
  int sockfd, rc, on = 1;
  int max_sd, new_sd;
  int desc_ready, end_server = FALSE;
  int close_conn;
  char buffer[BUFFER];
  struct sockaddr_in servaddr;
  // char* message = "Hello Client";
  fd_set master_set, working_set;
  struct timeval timeout;
  // pid_t pid;
  if (argc != 2) {
    printf("Input: %s <port>\n", argv[0]);
    return 0;
  }

  int PORT = atoi(argv[1]);

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  };

  rc = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char * ) & on, sizeof(on));
  if (rc < 0) {
    perror("setsockopt() failed");
    close(sockfd);
    exit(-1);
  }

  rc = ioctl(sockfd, FIONBIO, (char * ) & on);
  if (rc < 0) {
    perror("ioctl() failed");
    close(sockfd);
    exit(-1);
  }

  memset( & servaddr, 0, sizeof(servaddr));
  // memset(&cliaddr, 0, sizeof(cliaddr));

  servaddr.sin_family = AF_INET; // IPv4
  servaddr.sin_addr.s_addr = inet_addr(LOCALHOST);
  servaddr.sin_port = htons(PORT);

  if (bind(sockfd, (struct sockaddr * ) & servaddr, sizeof(servaddr)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
  if ((listen(sockfd, 5)) != 0) {
    printf("Listen failed...\n");
    exit(0);
  }
  // socklen_t cli_addr_size = sizeof(cliaddr);

  FD_ZERO( & master_set);
  max_sd = sockfd;
  FD_SET(sockfd, & master_set);
  timeout.tv_sec = 10;
  timeout.tv_usec = 0;
  do {
    memcpy( & working_set, & master_set, sizeof(master_set));
    printf("Waiting on select()...\n");
    rc = select(max_sd + 1, & working_set, NULL, NULL, & timeout);
    if (rc < 0) {
      perror("  select() failed");
      break;
    }
    if (rc == 0) {
      printf("  select() timed out.  End program.\n");
      break;
    }
    desc_ready = rc;
    for (int i = 0; i <= max_sd && desc_ready > 0; ++i) {
      if (FD_ISSET(i, & working_set)) {
        desc_ready -= 1;
        if (i == sockfd) {
          printf("  Listening socket is readable\n");
          do {
            new_sd = accept(sockfd, NULL, NULL);
            if (new_sd < 0) {
              if (errno != EWOULDBLOCK) {
                perror("  accept() failed");
                end_server = TRUE;
              }
              break;
            }
            printf("  New incoming connection - %d\n", new_sd);
            FD_SET(new_sd, & master_set);
            if (new_sd > max_sd)
              max_sd = new_sd;
          } while (new_sd != -1);
        } else {
          printf("  Descriptor %d is readable\n", i);
          close_conn = FALSE;
          do {
            rc = read(i, buffer, sizeof(buffer));
            if (rc < 0) {
              if (errno != EWOULDBLOCK) {
                perror("  recv() failed");
                close_conn = TRUE;
              }
              break;
            }
            if (rc == 0) {
              printf("  Connection closed\n");
              close_conn = TRUE;
              break;
            }
            printf("  Data received : %s\n", buffer);
            rc = write(i, buffer, sizeof(buffer));
            if (rc < 0) {
              perror("  send() failed");
              close_conn = TRUE;
              break;
            }

          } while (TRUE);
          if (close_conn) {
            close(i);
            FD_CLR(i, & master_set);
            if (i == max_sd) {
              while (FD_ISSET(max_sd, & master_set) == FALSE)
                max_sd -= 1;
            }
          }
        } /* End of existing connection is readable */
      } /* End of if (FD_ISSET(i, &working_set)) */
    } /* End of loop through selectable descriptors */

  } while (end_server == FALSE);
  for (int i = 0; i <= max_sd; ++i) {
    if (FD_ISSET(i, & master_set))
      close(i);
  }

}