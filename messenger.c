#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "client.h"
#include "server.h"
#include "defs.h"

int main(void) {
    int sockfd = 0;
    char name[INET6_ADDRSTRLEN] = "127.0.0.1";
    char port[6] = "0";
    pthread_t thread1;

    while(1) {
        printf("Peer-to-peer messenger\n");
        printf("- Configure local server\n");

        sockfd = configure_server(port);

        if (sockfd != 0) {
            pthread_create(&thread1, NULL, &accept_connections, (void *) &sockfd);
            break;
        }
    }
    
    printf("- Server up at: %s:%s\n", name, port);
    printf("- Establish connection to\n");

    while(1) {
        configure_connection(port);
    }

    printf("Exiting program...\n");

    return 0;
}
