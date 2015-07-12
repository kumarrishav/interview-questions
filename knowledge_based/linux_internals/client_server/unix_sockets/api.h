#ifndef MY_TCP_API_H
#define MY_TCP_API_H

#include <stddef.h>

#define BACKLOG_SZ 10
#define CLIENT_SOCK_DIR "/tmp"
#define STALE_TIME 10
#define CLIPERM S_IRWXU

int new_server(const char *name);
void destroy_server(int servfd);
int accept_client(int servfd);
int connect_to(const char *name);
int recv_info(int fd, void *buff, size_t buff_len);
int send_info(int fd, void *buff, size_t buff_len);
void disconnect_from(int fd);

#endif /* MY_TCP_API_H */
