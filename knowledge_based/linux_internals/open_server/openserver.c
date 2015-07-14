/* An openserver exec()'d by the client */

#include "oserver.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void handle_request(char *request) {
	char *buff;
	int omode;

	if ((buff = strdup(request)) == NULL) {
		perror("strdup(3) couldn't allocate memory");
		return;
	}

	if (sscanf(request, "open %s %d", buff, &omode) != 2) {
		fprintf(stderr, "Invalid request: %s\n", request);
		goto outbuff;
	}

	int fd = open(buff, omode);

	if (fd < 0) {
		int open_error = errno;
		if (send_errno(STDOUT_FILENO) < 0) {
			fprintf(stderr, "send_errno() failed attempting to notify client: %s\n",
				strerror(errno));
			fprintf(stderr, "Note: couldn't open %s: %s\n", buff, strerror(open_error));
		}
		goto outbuff;
	}

	if (send_fd(STDOUT_FILENO, fd) < 0) {
		fprintf(stderr, "send_fd() failed attempting to send fd for %s: %s\n",
			buff, strerror(errno));
		goto outclose;
	}

outclose:
	close(fd);
outbuff:
	free(buff);
}

int main(void) {
	char *linebuff = NULL;
	size_t linebuff_sz;
	ssize_t line_sz;

	errno = 0;
	while ((line_sz = getline(&linebuff, &linebuff_sz, stdin)) > 0) {
		if (linebuff[line_sz] == '\n') {
			linebuff[line_sz] = '\0';
		}

		handle_request(linebuff);

		errno = 0;
	}

	free(linebuff);

	if (errno != 0) {
		perror("getline(3) failed");
		exit(EXIT_FAILURE);
	}

	return 0;
}
