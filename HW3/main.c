#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include "network.h"
#include "lib.h"
#define MAX 30
#define N 256

typedef struct _account {
    char username[MAX];
    char password[MAX];
    int status;
    int signin;
    char homepage[MAX];
    struct _account *next;
}account;

account* head = NULL;

int checkSpace(char *str) {
    for(int i = 0; i < strlen(str); i++){
        if(str[i] == ' ') return 1;
    }
    return 0;
}

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
    for(account *temp = head; temp; temp = temp->next){
        if(strcmp(temp->username, username) == 0) {
            return 1;
        }
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

int checkAccount(char* password, char* username){
    for(account *temp = head; temp; temp = temp->next){
        if(strcmp(temp->password, password) == 0 && strcmp(temp->username, username) == 0) {
           return 1;
        }
    }
    return 0;
}

void setSignIn(char* username) {
    for(account *temp = head; temp; temp = temp->next){
        if(strcmp(temp->username, username) == 0) {
           temp->signin = 1;
           break;
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
		fprintf(fin,"%s %s %d %s\n",p->username, p->password, p->status, p->homepage);
		p = p->next;
	}
	fclose(fin);
}

void activeAccount(char* username) { // chuc nang 2
    setStatus(1, username);
    account* temp = head;
	writeFile(temp, username);
}

void blockAccount(char* username){ // chuc nang 3
    setStatus(0, username);
    account* temp = head;
	writeFile(temp, username);
}

void setNewPassword(char* username, char* password){ // chuc nang 5
    account *temp1 = head;
    while(temp1 != NULL){
		if (strcmp(temp1->username, username) == 0){
		    strcpy(temp1->password, password);
            temp1->signin = 0;
			break;
		}
		temp1 = temp1->next;
	}
    account* temp = head;
	writeFile(temp, username);
}

void checkBlock(char* username){ // chuc nang 4
    for(account *temp = head; temp; temp = temp->next){
        if(strcmp(temp->username, username) == 0) {
            switch(temp->status) {
                case 0: 
                    printf("Account is blocked\n");
                    break;
                case 1: 
                    printf("Account is active\n");
                    break;
                default:
                    printf("Account is idle\n");
                    break;
            }
            return;
        }
    }
}

void signOut(char* username){ 
    for(account *temp = head; temp; temp = temp->next){
        if(strcmp(temp->username, username) == 0 && temp->signin == 1) {
            printf("Goodbye hust");
            temp->signin = 0;
            return;
        }
    }
    printf("Account is not sign in");
}

void chucnang1() {
    char username[MAX], password[MAX], homepage[MAX];
    char temp;
    do {
       printf("Username : ");
        scanf("%c",&temp); // temp statement to clear buffer
        scanf("%[^\n]",username);
    }while(checkSpace(username));
    
    if(searchUsername(username)) {
        printf("Account existed");
    }else{
        do {
            printf("Password : ");
            scanf("%c",&temp); // temp statement to clear buffer
            scanf("%[^\n]", password);
        }while(checkSpace(password));
        do {
            printf("Homepage : ");
            scanf("%c",&temp); // temp statement to clear buffer
            scanf("%[^\n]", homepage);
        }while(checkSpace(homepage));
         if (isValidIp(homepage) == 0 || isHostName(homepage) == 0) {
            printf("Invalid Homepage\n");
        }
        account p;
        strcpy(p.password, password);
        strcpy(p.username, username);
        strcpy(p.homepage, homepage);
        p.status = 2;
        add(&head, makeNode(p));
        writeFileRegister(username, password, homepage);
        printf("Successful registration\n");
    }
}

void chucnang2() {
    char username[MAX], password[MAX];
    int count = 0;
    char code[MAX];
    char temp;
    do {
       printf("Username : ");
        scanf("%c",&temp); // temp statement to clear buffer
        scanf("%[^\n]",username);
    }while(checkSpace(username));
    if(!searchUsername(username)) {
        printf("Cannot find account");
    }else {
        do {
            do {
                printf("Password : ");
                scanf("%c",&temp); // temp statement to clear buffer
                scanf("%[^\n]", password);
            }while(checkSpace(password));
            if(checkAccount(password, username)) {
                do {
                    printf("Code : ");
                    scanf("%s", code);
                    if(strcmp(code, "20184152") == 0) {
                        activeAccount(username);
                        printf("Account is activated");
                        break;
                    }else {
                        printf("Account is not activated\n");
                        count++;
                    }
                }while(count < 4);
                break;
            }else {
                printf("Password is incorrect\n");
                count++;
            }
        }while(count < 3);
    }
    if(count == 3) {
        blockAccount(username);
        printf("Password is incorrect.\nAccount is blocked\n");
    }
    else if (count == 4){
        blockAccount(username);
        printf("Activation code is incorrect.\nAccount is blocked\n");
    }
}

void chucnang3() {
    char username[MAX], password[MAX];
    int count = 0;
    char temp;
    do {
       printf("Username : ");
        scanf("%c",&temp); // temp statement to clear buffer
        scanf("%[^\n]",username);
    }while(checkSpace(username));
    if(!searchUsername(username)) {
        printf("Cannot find account");
    }else {
        if(searchStatus(username)) {
            printf("Account is blocked\n");
            return;
        }
        do {
            do {
                printf("Password : ");
                scanf("%c",&temp); // temp statement to clear buffer
                scanf("%[^\n]", password);
            }while(checkSpace(password));
            if(checkAccount(password, username)) {
                printf("Hello hust");
                setSignIn(username);
                break;
            }else {
                printf("Password is incorrect\n");
                count++;
            }
        }while(count < 3);
    }
    if (count == 3){
        blockAccount(username);
        printf("Password is incorrect.\nAccount is blocked\n");
    }
}

void chucnang4() {
    char username[MAX];
    char temp;
    do {
        printf("Username : ");
        scanf("%c",&temp); // temp statement to clear buffer
        scanf("%[^\n]",username);
    }while(checkSpace(username));
    if(searchUsername(username) == 1) {
        account *tmp = getAccount(username);
        if(tmp->status == 2) {
            printf("Account is not activated.\n");
            return;
        } else if (tmp->status == 0) {
            printf("Account is blocked.\n");
            return;
        } 
        if(tmp->signin == 1) {
            printf("Account is active.\n");
        } else {
            printf("Account is not sign in\n");
        }
    } else {
        printf("Cannot find account\n");
    }
}

void chucnang5() {
    char username[MAX], password[MAX], newPassword[MAX];
    char temp;
    do {
        printf("Username : ");
        scanf("%c",&temp); // temp statement to clear buffer
        scanf("%[^\n]",username);
    }while(checkSpace(username));
    if(!searchUsername(username)) {
        printf("Cannot find account");
    }else {
        do {
            printf("Password : ");
            scanf("%c",&temp); // temp statement to clear buffer
            scanf("%[^\n]", password);
        }while(checkSpace(password));
         do {
            printf("NewPassword : ");
            scanf("%c",&temp); // temp statement to clear buffer
            scanf("%[^\n]", newPassword);
        }while(checkSpace(password));
        if(checkAccount(password, username)) {
            setNewPassword(username, newPassword);
            printf("Password is changed\n");
        }else {
            printf("Current password is incorrect. Please try again\n");
        }
    }
}

void chucnang6() {
    char username[MAX];
    char temp;
    do {
        printf("Username : ");
        scanf("%c",&temp); // temp statement to clear buffer
        scanf("%[^\n]",username);
    }while(checkSpace(username));
    if(searchUsername(username)) {
        signOut(username);
    }else {
        printf("Cannot find account");
    }
}

int main() {
    readFile();
    do{
		printf("\n---------------------------------------\n");
		printf("USER MANAGEMENT PROGRAM\n");
		printf("---------------------------------------\n");
		printf("1. Register\n");
        printf("2. Activate\n");
		printf("3. Sign in\n");
		printf("4. Search\n");
        printf("5. Change password\n");
		printf("6. Sign out\n");
        printf("7. Homepage with domain name\n");
        printf("8. Homepage with IP address\n");
		printf("Your choice (1-6), other to quit: \n");
        int choice = 0;
		scanf("%d",&choice);
        char username[MAX];
        char temp;
        switch(choice) {
            case 1: 
				chucnang1();
                break;
            
            case 2: 
                chucnang2();
                break;
            
            case 3: 
                chucnang3();
                break;
            
            case 4: 
                chucnang4(); 
                break;
            case 5: 
                chucnang5();
                break;
            
            case 6: 
                chucnang6();
                break;
            case 7:
                do {
                    printf("Username : ");
                    scanf("%c",&temp);
                    scanf("%[^\n]",username);
                }while(checkSpace(username));
                if(searchUsername(username)) {
                    account *tmp = getAccount(username);
                    if(tmp->status == 2) {
                        printf("Account is not activated.\n");
                    } else if (tmp->status == 0) {
                        printf("Account is blocked.\n");
                    }
                    if(tmp->signin == 1) {
                        showDomain(tmp->homepage);
                    }else {
                        printf("Account is not sign in\n");
                    }
                }else {
                    printf("Cannot find account\n");
                }
                break;
            case 8:
                do {
                    printf("Username : ");
                    scanf("%c",&temp);
                    scanf("%[^\n]",username);
                }while(checkSpace(username));
                if(searchUsername(username)) {
                    account *tmp = getAccount(username);
                    if(tmp->status == 2) {
                        printf("Account is not activated.\n");
                    } else if (tmp->status == 0) {
                        printf("Account is blocked.\n");
                    }
                    if(tmp->signin == 1) {
                        showIp(tmp->homepage);
                    }else {
                        printf("Account is not sign in\n");
                    }
                }else {
                    printf("Cannot find account\n");
                }
                break;
            default: return 0;
        }
    }while(1);
}