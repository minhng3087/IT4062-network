#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "lib.h"
#include "network.h"

void showIp(char *domain) {
    if(isHostName(domain)) {
        hostToIp(domain);
    }else if(isValidIp(domain)) {
        printf("Official IP: %s\n", domain);
    }
    else {
        printf("Not found information\n");
    }
}

void showDomain(char *ip) {
    if(isValidIp(ip)) {
        ipToHost(ip);
    }else if(isHostName(ip)) {
        printf("Offical name: %s\n", ip);
    }else {
        printf("Not found information\n");
    }
}