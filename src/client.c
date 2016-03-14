#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include "define.c"

int main(){
    int sock;
    struct sockaddr_in addr;
    
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    int i;for(i=0;i<100*10000;i++){
       char s[32]="";
       sprintf(s,"%d",i);
       sendto(sock, s, strlen(s), 0, (struct sockaddr *)&addr, sizeof(addr));
    }
    
    close(sock);
    
    return 0;
}
