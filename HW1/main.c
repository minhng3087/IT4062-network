#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define MAX 30
#define N 256

typedef struct _account {
    char username[MAX];
    char password[MAX];
    int status;
    int signin;
    struct _account *next;
}account;

typedef struct _list{
    account *head;
    account *tail;
} list;

void init(list *acc){
    acc->head = acc->tail = NULL;
}

int m = 0; // chua signin

account *makeNode(account temp) {
    account *p = (account *)malloc(sizeof(account));
    strcpy(p->username, temp.username);
    strcpy(p->password, temp.password);
    p->status = temp.status;
    p->signin = 0; // 0: chua signin, 1:signin
    p->next = NULL;
    return p;
}

void add(list *acc, account *p){
    if(acc->head == NULL) {
        (*acc).head = (*acc).tail = p;
        return;
    }
    else{
        (*acc).tail->next = p;
        (*acc).tail = p;
    }
    return;
}

void readFile(list* acc){
    account temp;
    FILE *fin = fopen("account.txt","r");
    char line[N];
    while(fgets(line, N, fin) != NULL){
        sscanf(line, "%s %s %d", temp.username, temp.password, &temp.status);
        add(acc, makeNode(temp));
    }
    fclose(fin);
}

void writeFileRegister(char* username, char* password) {
    FILE* fin = fopen("account.txt", "a");
    fprintf(fin,"\n%s %s 2",username, password);
    fclose(fin);
}

void printList(list* acc){
    for(account *p = acc->head; p; p = p->next){
        printf("%s %s %d %d\n", p->username, p->password, p->status, p->signin);
    }
}

int searchUsername(list* acc, char* username){
    for(account *temp = acc->head; temp; temp = temp->next){
        if(strcmp(temp->username, username) == 0) {
            return 1;
        }
    }
    return 0;
}

int searchStatus(list* acc, char* username){
    for(account *temp = acc->head; temp; temp = temp->next){
        if(strcmp(temp->username, username) == 0 && temp->status == 0){
            return 1;
        }
    }
    return 0;
}

int checkAccount(list* acc, char* password, char* username){
    for(account *temp = acc->head; temp; temp = temp->next){
        if(strcmp(temp->password, password) == 0 && strcmp(temp->username, username) == 0) {
           return 1;
        }
    }
    return 0;
}

void setSignIn(list* acc, char* username) {
    for(account *temp = acc->head; temp; temp = temp->next){
        if(strcmp(temp->username, username) == 0) {
           temp->signin = 1;
           break;
        }
    }
}

void setStatus(list* acc, int status, char* username){
    account *temp1 = acc->head;
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

void activeAccount(list* acc, char* username) { // chuc nang 2
    setStatus(acc, 1, username);
    account* temp = acc->head;
	writeFile(temp, username);
}

void blockAccount(list* acc,char* username){ // chuc nang 3
    setStatus(acc, 0, username);
    account* temp = acc->head;
	writeFile(temp, username);
}

void setNewPassword(list* acc,char* username, char* password){ // chuc nang 5
    account *temp1 = acc->head;
    while(temp1 != NULL){
		if (strcmp(temp1->username, username) == 0){
		    strcpy(temp1->password, password);
            temp1->signin = 0;
			break;
		}
		temp1 = temp1->next;
	}
    account* temp = acc->head;
	writeFile(temp, username);
}

void checkBlock(list* acc, char* username){ // chuc nang 4
    for(account *temp = acc->head; temp; temp = temp->next){
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

void signOut(list* acc, char* username){ 
    for(account *temp = acc->head; temp; temp = temp->next){
        if(strcmp(temp->username, username) == 0 && temp->signin == 1) {
            printf("Goodbye hust");
            temp->signin = 0;
            return;
        }
    }
    printf("Account is not sign in");
}

void chucnang1(list* acc) {
    char username[MAX], password[MAX];
    printf("Username : ");
    scanf("%s", username);
    if(searchUsername(acc, username)) {
        printf("Account existed");
    }else{
        printf("Password : ");
        scanf("%s", password);
        account p;
        strcpy(p.password, password);
        strcpy(p.username, username);
        p.status = 2;
        add(acc, makeNode(p));
        writeFileRegister(username, password);
        printf("Successful registration\n");
    }
}

void chucnang2(list* acc) {
    char username[MAX], password[MAX];
    int count = 0;
    char code[MAX];
    printf("Username : ");
    scanf("%s", username);
    if(!searchUsername(acc, username)) {
        printf("Cannot find account");
    }else {
        do {
            printf("Password : ");
		    scanf("%s", password);
            if(checkAccount(acc, password, username)) {
                do {
                    printf("Code : ");
                    scanf("%s", code);
                    if(strcmp(code, "20184152") == 0) {
                        activeAccount(acc, username);
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
        blockAccount(acc, username);
        printf("Password is incorrect.\nAccount is blocked\n");
    }
    else if (count == 4){
        blockAccount(acc, username);
        printf("Activation code is incorrect.\nAccount is blocked\n");
    }
}

void chucnang3(list* acc) {
    char username[MAX], password[MAX];
    int count = 0;
    printf("Username : ");
    scanf("%s", username);
    if(!searchUsername(acc, username)) {
        printf("Cannot find account");
    }else {
        if(searchStatus(acc, username)) {
            printf("Account is blocked\n");
            return;
        }
        do {
            printf("Password : ");
		    scanf("%s", password);
            if(checkAccount(acc, password, username)) {
                printf("Hello hust");
                setSignIn(acc, username);
                m = 1;
                break;
            }else {
                printf("Password is incorrect\n");
                count++;
            }
        }while(count < 3);
    }
    if (count == 3){
        blockAccount(acc, username);
        printf("Password is incorrect.\nAccount is blocked\n");
    }
}

void chucnang4(list* acc) {
    char username[MAX], password[MAX];
    int count = 0;
    printf("Username : ");
    scanf("%s", username);
    if(searchUsername(acc, username)) {
        checkBlock(acc, username);
    }else {
        printf("Cannot find account");
    }
}

void chucnang5(list* acc) {
    char username[MAX], password[MAX], newPassword[MAX];
    printf("Username : ");
    scanf("%s", username);
    if(!searchUsername(acc, username)) {
        printf("Cannot find account");
    }else {
        printf("Password : ");
	    scanf("%s", password);
        printf("NewPassword : ");
        scanf("%s", newPassword);
        if(checkAccount(acc, password, username)) {
            setNewPassword(acc, username, newPassword);
            printf("Password is changed\n");
        }else {
            printf("Current password is incorrect. Please try again\n");
        }
    }
}

void chucnang6(list* acc) {
    char username[MAX], password[MAX];
    printf("Username : ");
    scanf("%s", username);
    if(searchUsername(acc, username)) {
        signOut(acc, username);
    }else {
        printf("Cannot find account");
    }
}

int main() {
    list* acc;init(acc);
    readFile(acc);
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
		printf("Your choice (1-6), other to quit: \n");
        int choice = 0;
		scanf("%d",&choice);
        switch(choice) {
            case 1: {
				chucnang1(acc);
                break;
            }
            case 2: {
                chucnang2(acc);
                break;
            }
            case 3: {
                chucnang3(acc);
                break;
            }
            case 4: {
                if(m == 0) {
                    printf("Please sigin\n");
                }else {
                    chucnang4(acc);
                }
                break;
            }
            case 5: {
                chucnang5(acc);
                break;
            }
            case 6: {
                chucnang6(acc);
                break;
            }
            default: return 0;
        }
    }while(1);
}