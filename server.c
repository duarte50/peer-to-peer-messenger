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

#include "server.h"
#include "defs.h"

int configure_server(char *port) {
    printf("Port: ");
    fgets(port, 6, stdin);
    port[strcspn(port, "\n")] = 0;

    return run_server(port);
}

int run_server(char *port) {
    int sockfd;
    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int status = getaddrinfo(NULL, port, &hints, &servinfo);

    if (status != 0) {
        fprintf(stderr, "[Server] getaddrinfo error: %s\n", gai_strerror(status));
        return 0;
    }

    int optval = 1;

    for (p = servinfo; p != NULL; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

        if (sockfd == -1) {
            perror("[Server] socket error");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval) == -1) {
            close(sockfd);
            perror("[Server] setsockopt error");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("[Server] bind error");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo);

    if (p == NULL) {
        perror("[Server] cannot bind");
        return 0;
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("[Server] listen error");
        return 0;
    }

    struct sigaction sa;

    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("[Server] sigaction error");
        return 0;
    }

    return sockfd;
}

void *accept_connections(void *ptr) {
    int sockfd = *(int *) ptr;
    int newfd;
    char client_ipaddr[INET6_ADDRSTRLEN];
    struct sockaddr_storage clientaddr;
    socklen_t sin_size;

    while(1) {
        sin_size = sizeof clientaddr;
        newfd = accept(sockfd, (struct sockaddr *) &clientaddr, &sin_size);

        if (newfd == -1) {
            perror("[Server] accept error");
            continue;
        }

        inet_ntop(
            clientaddr.ss_family,
            get_in_addr((struct sockaddr *) &clientaddr),
            client_ipaddr,
            sizeof client_ipaddr
        );

        if (!fork()) {
            close(sockfd);

            char buffer[MAXDATASIZE];
            int numbytes = recv(newfd, buffer, MAXDATASIZE - 1, 0);

            if (numbytes == -1) {
                perror("[Server] recv error");
                return NULL;
            }

            char *client_port;
            char *client_message;
            buffer[numbytes] = '\0';

            client_port = strtok(buffer, ":");
            client_message = buffer + strlen(client_port) + 1;

            printf("\n[%s:%s] sent: %s\n", client_ipaddr, client_port, client_message);

            close(newfd);

            return NULL;
        }

        close(newfd);
    }
}

void sigchld_handler(int s) {
    int saved_errno = errno;

    while (waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}
