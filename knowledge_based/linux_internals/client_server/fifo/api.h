#ifndef MY_TCP_API_H
#define MY_TCP_API_H

#include <stdio.h>

#define FIFO_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

struct comm_channel {
	char *readpath;
	char *writepath;
	FILE *readchan;
	FILE *writechan;
};

struct server {
	char *servname;
	FILE *file;
};

int new_server(const char *servname, struct server *oserv);
void destroy_server(struct server *server);
int accept_client(struct server *server, struct comm_channel *ochan);
int connect_to(const char *servname, struct comm_channel *ochan);
void disconnect_from(struct comm_channel *ochan);
int recv_info(struct comm_channel *chan, void *ptr, size_t len);
int send_info(struct comm_channel *chan, void *ptr, size_t len);

#endif
