#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 4

int main(int argc, char **argv) {
    int MasterSocket = socket(
        AF_INET /* IPv4*/,  
        SOCK_STREAM /* TCP */,
        IPPROTO_TCP
    );

    if (MasterSocket == -1) {
        perror("cannot create socket");
        exit(EXIT_FAILURE);
    }
 
    struct sockaddr_in SockAddr;
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_port = htons(12345);
    SockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(MasterSocket, (struct sockaddr *)(&SockAddr), sizeof(SockAddr)) == -1) {
        perror("bind failed");
        close(MasterSocket);
        exit(EXIT_FAILURE);
    }

    if (listen(MasterSocket, SOMAXCONN) == -1) {
        perror("listen failed");
        close(MasterSocket);
        exit(EXIT_FAILURE);
    }

    char Buffer[BUFFER_SIZE] = {0};

    while(1) {
        int SlaveSocket = accept(MasterSocket, 0, 0);
        if (SlaveSocket <= 0) {
            printf("accept failed");
            continue;
        }

        memset(Buffer, 0, BUFFER_SIZE);

        unsigned counter = 0;
        while (counter < BUFFER_SIZE) {
            int res = recv(SlaveSocket, Buffer + counter, BUFFER_SIZE - counter, MSG_NOSIGNAL);
            if (res > 0) {
                counter += res;
            }
        }

        send(SlaveSocket, Buffer, BUFFER_SIZE, MSG_NOSIGNAL);

        shutdown(SlaveSocket, SHUT_RDWR);
        close(SlaveSocket);

        printf("%s\n", Buffer);
    }
}