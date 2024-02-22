#define BACKLOG 5

int configure_server(char *port);
int run_server(char *port);
void *accept_connections(void *ptr);
void *get_in_addr(struct sockaddr *sa);
void sigchld_handler(int s);
