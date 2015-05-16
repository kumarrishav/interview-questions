/* Write a program that receives a file descriptor as an argument and prints
 * the file status flags.
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s <fd>\n", argv[0]);
		return 128;
	}

	errno = 0;
	int fd = strtol(argv[1], NULL, 10);
	if (errno != 0) {
		perror("Invalid file descriptor");
		return 128;
	}
	if (fd < 0) {
		fprintf(stderr, "Please use a positive file descriptor");
		return 128;
	}

	int flags = fcntl(fd, F_GETFL, 0);
	if (flags < 0) {
		perror("Could not get file status flags");
		return 128;
	}

	switch (flags & O_ACCMODE) {
	case O_RDONLY:
		printf("read-only");
		break;
	case O_WRONLY:
		printf("write-only");
		break;
	case O_RDWR:
		printf("read-write");
		break;
	default:
		printf("Unknown flag");
		break;
	}

	if (flags & O_APPEND) {
		printf(", append");
	}

	if (flags & O_NONBLOCK) {
		printf(", non-blocking");
	}

	if (flags & O_SYNC) {
		printf(", synchronous writes");
	}

	printf("\n");

	return 0;
}
