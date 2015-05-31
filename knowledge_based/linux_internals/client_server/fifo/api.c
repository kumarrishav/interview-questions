#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define FIFO_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

struct comm_channel {
	char *readpath;
	char *writepath;
	int readchan;
	int writechan;
};

struct server {
	char *servname;
	FILE *file;
};

static int create_fifos(struct comm_channel *ochan);
static int open_fifos(struct comm_channel *ochan, int read_first);

int new_server(const char *servname, struct server *oserv) {
	if (mkfifo(servname, FIFO_MODE) < 0) {
		return -1;
	}

	/* We open for reading and writing to avoid dealing with EOF on the FIFO
	 * when no clients are contacting the server
	 */
	oserv->file = fopen(servname, "r+");
	if (oserv->file == NULL) {
		unlink(servname);
		return -1;
	}

	oserv->servname = strdup(servname);
	if (oserv->servname == NULL) {
		fclose(oserv->file);
		unlink(servname);
		return -1;
	}

	return 0;
}

void destroy_server(struct server *server) {
	unlink(server->servname);
	fclose(server->file);
	free(server->servname);
	server->servname = NULL;
	server->file = NULL;
}

int accept_client(struct server *server, struct comm_channel *ochan) {
	static char buf[32];

	if (fgets(buf, sizeof(buf), server->file) == NULL) {
		return -1;
	}

	long pid;
	if (sscanf(buf, "%ld", &pid) != 1) {
		return -1;
	}

	ochan->readpath = malloc(strlen(server->servname)+strlen(buf)+4);
	ochan->writepath = malloc(strlen(server->servname)+strlen(buf)+4);

	if (ochan->readpath == NULL || ochan->writepath == NULL) {
	        free(ochan->readpath);
		free(ochan->writepath);
		return -1;
	}

	sprintf(ochan->readpath, "%s_%ld_r", server->servname, pid);
	sprintf(ochan->writepath, "%s_%ld_w", server->servname, pid);

	if (open_fifos(ochan, 0) < 0) {
	        free(ochan->readpath);
		free(ochan->writepath);
		return -1;
	}

	return 0;
}

int connect_to(const char *servname, struct comm_channel *ochan) {
	ochan->readpath = malloc(strlen(servname)+32+4);
	ochan->writepath = malloc(strlen(servname)+32+4);

	if (ochan->readpath == NULL || ochan->writepath == NULL) {
		goto free_cleanup;
	}

	sprintf(ochan->readpath, "%s_%ld_w", servname, (long) getpid());
	sprintf(ochan->writepath, "%s_%ld_r", servname, (long) getpid());

	if (create_fifos(ochan) < 0) {
		goto unlink_cleanup;
	}

	FILE *server = fopen(servname, "r+");

	if (server == NULL) {
		goto unlink_cleanup;
	}

	fprintf(server, "%ld\n", (long) getpid());
	fflush(server);

	if (open_fifos(ochan, 1) < 0) {
		goto unlink_cleanup;
	}

	unlink(ochan->readpath);
	unlink(ochan->writepath);
	return 0;

unlink_cleanup:
	unlink(ochan->readpath);
	unlink(ochan->writepath);
free_cleanup:
	free(ochan->readpath);
	free(ochan->writepath);

	return -1;
}

void disconnect_from(struct comm_channel *ochan) {
	unlink(ochan->writepath);
	unlink(ochan->readpath);
	free(ochan->readpath);
	free(ochan->writepath);
	close(ochan->writechan);
	close(ochan->readchan);
}

int recv_info(struct comm_channel *chan, void *ptr, size_t len) {
	return read(chan->readchan, ptr, len);
}

int send_info(struct comm_channel *chan, void *ptr, size_t len) {
	return write(chan->writechan, ptr, len);
}

static int create_fifos(struct comm_channel *ochan) {
	if (mkfifo(ochan->readpath, FIFO_MODE) < 0) {
		return -1;
	}

	if (mkfifo(ochan->writepath, FIFO_MODE) < 0) {
		unlink(ochan->readpath);
		return -1;
	}

	return 0;
}

static int open_fifos(struct comm_channel *ochan, int read_first) {

	if (read_first) {
		ochan->readchan = open(ochan->readpath, O_RDONLY);
		ochan->writechan = open(ochan->writepath, O_WRONLY);
	} else {
		ochan->writechan = open(ochan->writepath, O_WRONLY);
		ochan->readchan = open(ochan->readpath, O_RDONLY);
	}

	if (ochan->readchan < 0 || ochan->writechan < 0) {
		if (ochan->readchan >= 0) {
			close(ochan->readchan);
		}
		if (ochan->writechan >= 0) {
			close(ochan->writechan);
		}
		return -1;
	}

	return 0;
}
