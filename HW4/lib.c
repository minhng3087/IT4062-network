#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "lib.h"

int isValidIp(char *ipAddress)
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ipAddress, &(sa.sin_addr));
    return result != 0;
}

int isHostName(char *host) {
    if(host == NULL || isalpha(*host) == 0) return 0;
    return 1;
}

void hostToIp(char *host)
{
	struct hostent *he;
    struct in_addr **addr_list;
    he = gethostbyname(host);
    if ( he == NULL) {
        printf("Not found information\n");
    }else {
        addr_list = (struct in_addr **) he->h_addr_list;
        printf("Official IP: %s\n", inet_ntoa(*addr_list[0]));
        printf("Alias IP:\n");
        for (int i = 1; addr_list[i] != NULL; i++){
            printf("%s\n", inet_ntoa(*addr_list[i]));
        }
    }
}

void ipToHost(char *ip) {
    
    struct in_addr addr;
    int check = inet_aton(ip, &addr);
    struct hostent *host;
    char** name;
    host = gethostbyaddr(&addr, sizeof(addr), AF_INET);

    if (host  == NULL || !check) {
        printf("Not found information\n");
    }
    else {
        printf("Official name: %s\n", host->h_name);
        printf("Alias name:\n");
        name = host->h_aliases;
        while(*name) {
            printf(" %s", *name);
            name++;
        }
    }
}

char* delete_space(char *str,char *result) {
  int pos = 0;
  const int len = strlen(str); 
  for(int i = 0; i < len; i++) {
      if (str[0] == '\n') {
        result[0] = '\n';
        result[1] = '\0';
        break;
      }else {
        char c = str[i];
        if (c == '\r' || c == '\n' || c == ' ') {
            continue;
        }
        result[pos++] = c;
      }
  }
  return result;
}