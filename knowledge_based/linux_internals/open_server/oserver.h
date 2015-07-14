#ifndef OSERVER_H
#define OSERVER_H

int send_fd(int sockfd, int fd_to_send);
int send_errno(int sockfd);
int recv_fd(int sockfd);

#endif /* OSERVER_H */
