/* A brief illustration of record locks */

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len) {
	struct flock lock;

	lock.l_start = offset;
	lock.l_len = len;
	lock.l_whence = whence;
	lock.l_type = type;

	return fcntl(fd, cmd, &lock);
}

#define read_lock(fd, offset, whence, len) \
	lock_reg(fd, F_SETLK, F_RDLCK, (offset), (whence), (len))
#define readw_lock(fd, offset, whence, len) \
	lock_reg(fd, F_SETLKW, F_RDLCK, (offset), (whence), (len))
#define write_lock(fd, offset, whence, len) \
	lock_reg(fd, F_SETLK, F_WRLCK, (offset), (whence), (len))
#define writew_lock(fd, offset, whence, len) \
	lock_reg(fd, F_SETLKW, F_WRLCK, (offset), (whence), (len))
#define un_lock(fd, offset, whence, len) \
	lock_reg(fd, F_SETLK, F_UNLCK, (offset), (whence), (len))

static const char buf[] = "A message from a process.\n";

void print_msgs(const char *name, int fd) {

	int i;
	for (i = 0; i < 200; i++) {
		if (writew_lock(fd, 0, SEEK_SET, 0) < 0) {
			perror("Unable to acquire lock");
			return;
		}

		write(fd, name, strlen(name));
		write(fd, buf, sizeof(buf)-1);

		un_lock(fd, 0, SEEK_SET, 0);
	}

}

int main(void) {
	char template[] = "lockexampleXXXXXX";
	int tmpfd = mkstemp(template);

	if (tmpfd < 0) {
		perror("Couldn't create temporary file");
		exit(EXIT_FAILURE);
	}

	printf("Created temporary file %s\n", template);

	pid_t f = fork();

	if (f < 0) {
		perror("fork() failed");
		exit(EXIT_FAILURE);
	}

	if (f == 0) {
		print_msgs("child: ", tmpfd);
	} else {
		print_msgs("parent: ", tmpfd);
	}

	printf("Press any key to exit\n");

	getchar();

	if (f != 0 && unlink(template) < 0) {
		fprintf(stderr, "unlink() failed on %s: %s\n", template, strerror(errno));
		exit(EXIT_FAILURE);
	}

	return 0;
}
