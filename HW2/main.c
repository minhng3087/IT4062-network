#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib.h"
#define MAXCHAR 100

int main(int argc, char *argv[]) {
    if (argc <= 2) {
        printf("Missing parameters!\n");
        return -1;
    }
    int param1;
    char param2[MAXCHAR];

    param1 = atoi(argv[1]);
    strcpy(param2, argv[2]);

    if (param1 == 1) {
        if(isValidIp(param2)) {
            ipToHost(param2);
        }else if(!isValidIp(param2)) {
            if(isHostName(param2)) {
                printf("Wrong parameter\n");
            }else {
                printf("Not found information\n");
            }
        }
    } else if (param1 == 2 && isHostName(param2)){
        hostToIp(param2);
    }else {
        printf("Wrong parameter\n");
    }
}