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
#include <assert.h>
#include <time.h>
#include <sys/time.h>
#define MAX 30
#define N 256
#define LOCALHOST "127.0.0.1"
#define BUFFER 1024

typedef struct _message {
    char type[10];
    char desc[255];
}message;

char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}

void write_file(char* filename, char message[MAX]) {
    FILE* fp = fopen(filename,"a");
    time_t now = time(NULL);
    fprintf(fp,"%s => %s\n",ctime(&now), message);
    fclose(fp);
}

int main(int argc, char **argv) {
    int sockfd, flag = 0, connfd;
    char request[BUFFER], response[BUFFER];
    char type[MAX], text_log[MAX];
    char username[MAX];
    char name_file[MAX];
    struct sockaddr_in servaddr, cliaddr;
    char** tokens;
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
    printf("Server started on %s:%d\n", LOCALHOST, PORT);
    while(1) {
        if ((connfd = accept(sockfd, (struct sockaddr*)&cliaddr, &cli_addr_size)) < 0 ) {
            perror("server accept failed...\n");
            exit(EXIT_FAILURE);
        }
        printf("Connection accepted from %s:%d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
        if ((pid = fork()) == 0) {
            close( sockfd );
            sockfd = -1;
            while(read(connfd, request, sizeof(request)) > 0) {
                if(strcmp(request, "bye") == 0) {
                    flag = 3;
                }
                tokens = str_split(request, ' ');
                strcpy(type, tokens[0]);
                if (strcmp(type, "login") == 0) {
                    strcpy(username, tokens[1]);
                    flag = 1;
                }
                if (strcmp(type, "text") == 0)  {
                    memset(text_log, 0, sizeof(text_log));
                    strcpy(name_file, username);
                    strcat(name_file, ".txt");
                    if (tokens){
                        for (int i = 1; *(tokens + i); i++) {
                            strcat(text_log, *(tokens + i));
                            strcat(text_log, " ");
                            free(*(tokens + i));
                        }
                    }
                    flag = 2;
                }
                if(strcmp(type, "logout") == 0) {
                    flag = 3;
                }

                switch(flag) {
                    case 1: 
                        strcpy(response, "Hello ");
                        strcat(response, username);
                        break;
                    case 2:
                        if(strcmp(text_log, "") == 0) {
                            break;
                        }
                        write_file(name_file, text_log);
                        strcpy(response, "Write to user ");
                        strcat(response, username);
                        strcat(response, " successfully");
                        break;
                    case 3:
                        strcpy(response, "Goodbye ");
                        strcat(response, username);
                        memset(username, 0, sizeof(username));
                        memset(type, 0, sizeof(type));
                        flag = 0;
                        break;
                }
                printf("Data received: %s\n" , type);
                free(tokens);
                write(connfd, response, sizeof(response));
            }
        }

    }
    return 0;
    
}
