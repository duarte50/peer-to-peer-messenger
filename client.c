#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "client.h"
#include "defs.h"

void configure_connection(char *port) {
    char hostname[127];
    char hostport[6];

    while (1) {
        printf("Hostname: ");
        fgets(hostname, 127, stdin);
        hostname[strcspn(hostname, "\n")] = 0;

        printf("Host port: ");
        fgets(hostport, 6, stdin);
        hostport[strcspn(hostport, "\n")] = 0;

        if (strlen(hostname) > 0 && strlen(hostport) > 0) {
            break;
        }
    }

    run_client(hostname, hostport, port);
}

void run_client(char *hostname, char *hostport, char *my_port) {
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(hostname, hostport, &hints, &servinfo);

    if (status != 0) {
        fprintf(stderr, "[Client] getaddrinfo error: %s\n", gai_strerror(status));
        return;
    }

    for (p = servinfo; p != NULL; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

        if (sockfd == -1) {
            perror("[Client] socket error");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "[Client] cannot connect\n");
        return;
    }

    inet_ntop(
        p->ai_family,
        get_in_addr((struct sockaddr *) p->ai_addr),
        s,
        sizeof s
    );

    freeaddrinfo(servinfo);

    char message[MAXDATASIZE];
    int my_port_len = strlen(my_port);

    printf("Message to [%s:%s]: ", hostname, hostport);
    strncpy(message, my_port, my_port_len);
    message[my_port_len] = ':';
    fgets(message + my_port_len + 1, 127, stdin);

    message[strcspn(message, "\n")] = 0;

    if (send(sockfd, message, 100, 0) == -1) {
        perror("[Client] send error");
    }

    printf("Message sent\n");

    close(sockfd);
}