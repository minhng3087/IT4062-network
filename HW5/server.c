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
#include "network.h"
#include "lib.h"
#define MAX 30
#define N 256
#define LOCALHOST "127.0.0.1"
#define BUFFER 1024

typedef struct _account {
    char username[MAX];
    char password[MAX];
    int status;
    int signin;
    char homepage[MAX];
    struct _account *next;
}account;

account* head = NULL;

account *makeNode(account temp) {
    account *p = (account *)malloc(sizeof(account));
    strcpy(p->username, temp.username);
    strcpy(p->password, temp.password);
    strcpy(p->homepage, temp.homepage);
    p->status = temp.status;
    p->signin = 0; // 0: chua signin, 1:signin
    p->next = NULL;
    return p;
}

account *getAccount(char* username) {
    account *tmp = head;
    while (tmp != NULL) {
        if (strcmp(tmp->username, username) == 0) {
            return tmp;
        }
        tmp = tmp->next;
    }
    return NULL;
}

void add(account **head, account *node){
    if( *head == NULL) {
        *head = node;
    } else {
        account *tmp = *head;
        while (tmp->next != NULL) tmp = tmp->next;
        tmp->next = node;
    }
}

void printList() {
    account *tmp = head;
    while (tmp != NULL) {
        printf("%s\n", tmp->username);
        tmp = tmp->next;
    }
}

int checkString(char *str){
    while (*str) {
        if(!isalnum(*str) || *str == ' ') return 0;
        str++;
    }
    return 1;
}


void readFile(){
    account temp;
    FILE *fin = fopen("account.txt","r");
    char line[N];
    while(fgets(line, N, fin) != NULL){
        sscanf(line, "%s %s %d %s", temp.username, temp.password, &temp.status, temp.homepage);
        add(&head,makeNode(temp));
    }
    fclose(fin);
}

void writeFileRegister(char* username, char* password, char* homepage) {
    FILE* fin = fopen("account.txt", "a");
    fprintf(fin,"\n%s %s 2 %s",username, password, homepage);
    fclose(fin);
}

int searchUsername(char* username){
    account* temp = head;
    while(temp != NULL) {
        if(strcmp(temp->username, username) == 0) {
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}

int searchStatus(char* username){
    for(account *temp = head; temp; temp = temp->next){
        if(strcmp(temp->username, username) == 0 && temp->status == 0){
            return 1;
        }
    }
    return 0;
}

account* checkAccount(char* password, char* username){
    for(account *temp = head; temp; temp = temp->next){
        if(strcmp(temp->password, password) == 0 && strcmp(temp->username, username) == 0) {
           return temp;
        }
    }
    return NULL;
}

void checkBlock(char* username){ 
    for(account *temp = head; temp; temp = temp->next){
        if(strcmp(temp->username, username) == 0) {
            if(temp->status != 1) {
                printf("Account not ready\n");
            }
            return;
        }
    }
}


void setStatus(int status, char* username){
    account *temp1 = head;
    while(temp1 != NULL){
		if (strcmp(temp1->username, username) == 0){
			temp1->status = status;
			break;
		}
		temp1 = temp1->next;
	}
}

void writeFile(account* p, char* username) {
	FILE *fin = fopen("account.txt","w+");
	while(p!=NULL){
		fprintf(fin,"%s %s %d\n",p->username, p->password, p->status);
		p = p->next;
	}
	fclose(fin);
}


void blockAccount(char* username){ // chuc nang 3
    setStatus(0, username);
    account* temp = head;
	writeFile(temp, username);
}

void setSignIn(char* username) {
    for(account *temp = head; temp; temp = temp->next){
        if(strcmp(temp->username, username) == 0) {
           temp->signin = 1;
           break;
        }
    }
}


void setNewPassword(char* username, char* password){ // chuc nang 5
    account *temp1 = head;
    while(temp1 != NULL){
		if (strcmp(temp1->username, username) == 0){          
		    strcpy(temp1->password, password);
			break;
		}
		temp1 = temp1->next;
	}
    account* temp = head;
	writeFile(temp, username);
}

void signOut(char* username, char* result){
    for(account *temp = head; temp; temp = temp->next){
        if(strcmp(temp->username, username) == 0 && temp->signin == 1) {
            strcpy(result, "Goodbye ");
            strcat(result, username);
            temp->signin = 0;
            return;
        }
    }
    strcpy(result, "Account is not sign in");
}

void encodePass(char str[MAX], char *result) {
    char number[MAX], alpha[MAX];
    int x = 0, y = 0;
    for(int i = 0; i < strlen(str); i++) {
        if(isalpha(str[i])){
            alpha[y++] = str[i];
        }else if(isdigit(str[i])) {
            number[x++] = str[i];
        } 
    }
    number[x] = '\0';
    alpha[y] = '\0';
    if (number[0] == '\0') {
        strcpy(result, alpha);
        return;
    }else if(alpha[0] == '\0') {
        strcpy(result, number);
        return;
    }else {
        strcpy(result, number);
        strcat(result, "\n");
        strcat(result, alpha);
    }
}


int main(int argc, char **argv) {
    int sockfd, flag = 1, count = 0, connfd;
    char request[BUFFER], response[BUFFER];
    char username[MAX], password[MAX], newpass[MAX];
    struct sockaddr_in servaddr, cliaddr;
    if (argc != 2) {
        printf("Input: %s <port>\n", argv[0]);
        return 0;
    }
    readFile();

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
    if ((connfd = accept(sockfd, (struct sockaddr*)&cliaddr, &cli_addr_size)) < 0 ) {
        perror("server accept failed...\n");
        exit(EXIT_FAILURE);
    }
    printf("Server started on %s:%d\n", LOCALHOST, PORT);
    while(1) {
        // int n = recvfrom(sockfd, request, BUFFER, MSG_WAITALL, (struct sockaddr *) &cliaddr, &len);
        // request[n] = '\0';
        read(connfd, request, sizeof(request));
        if(strcmp(request, "bye") == 0) {
           flag = 4;
        }
        printf("Data received: %s\n" , request);

        switch(flag) {
            case 1:
                strcpy(username, request);
                if(searchUsername(username) == 1) {
                    strcpy(response, "Insert password");
                    flag++;
                }else {
                    strcpy(response, "Cannot find account\n");
                    strcat(response, "Input username again: ");
                }
                break;
            case 2:
                strcpy(password, request);
                account* temp = checkAccount(password, username);
                if(temp != NULL) {
                    if(temp->status == 1) {
                        strcpy(response, "OK\n");
                        strcat(response, "Input new password: ");
                        setSignIn(username);
                        count = 0;
                        flag++;
                    }else if(temp->status == 0 || temp->status == 2) {
                        strcpy(response, "Account not ready\n");
                        strcat(response, "Input username again: ");
                        flag = 1;
                    }
                }else {
                    strcpy(response, "Not OK");
                    count++;
                }
                if(count == 3) {
                    strcpy(response, "Account is blocked\n");
                    strcat(response, "Input username again: ");
                    blockAccount(username);
                    flag = 1;
                }
                break;
             case 3:    
                strcpy(newpass, request);
                if(checkString(newpass) == 0) {
                    strcpy(response, "Error\n");
                    strcat(response, "Input newpass again: ");
                }else {
                    setNewPassword(username, newpass);
                    encodePass(newpass, response);
                }
                break;
            case 4: 
                signOut(username, response);
                strcat(response, "\n");
                strcat(response, "Input username: ");
                memset(username, 0, sizeof(username));
                memset(password, 0, sizeof(password));
                flag = 1;
                break;
        }
        write(connfd, response, sizeof(response));

    }
    close(connfd);
    close(sockfd);
    return 0;
    
}
