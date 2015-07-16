#ifndef OSERVER_H
#define OSERVER_H

#define _GNU_SOURCE
#include <sys/socket.h>

int send_fd(int sockfd, int fd_to_send);
int send_errno(int sockfd);
int recv_fd(int sockfd, struct ucred *server);

#endif /* OSERVER_H */
