#include<stdio.h>
#include<sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<string.h>

#define PORT 8080

int main(){
    int sockfd;
    struct sockaddr_in serverAddr, clientAddr;
    char buffer[1024];
    int n;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(sockfd, (const struct sockaddr*)&serverAddr, sizeof(serverAddr));

    printf("UDP Echo Server running\n");

    while(1){
        int len = sizeof(clientAddr);
        n = (int)recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*)&clientAddr, &len);
        buffer[n] = '\0';
        printf("Received message from %s:%d : %s\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), buffer);
        sendto(sockfd, buffer, n, 0, (struct sockaddr*)&clientAddr, sizeof(clientAddr));
    }

    return 0;
}
