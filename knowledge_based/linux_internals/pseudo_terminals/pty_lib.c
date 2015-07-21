#define _XOPEN_SOURCE
#include "pty_lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

int posix_openpt(int flags) {
	return open("/dev/ptmx", flags);
}

int ptym_open(char *pts_name, size_t buff_len) {

	if (pts_name == NULL) {
		errno = EFAULT;
		return -1;
	}
		
	int ptym_fd;

	if ((ptym_fd = posix_openpt(O_RDWR)) < 0)
		return -1;

	char *pts_ptr = ptsname(ptym_fd);
	strncpy(pts_name, pts_ptr, buff_len);

	if (buff_len == 0 || pts_name[buff_len-1] != '\0') {
		errno = ENAMETOOLONG;
		goto errout;
	}

	if (grantpt(ptym_fd) < 0)
		goto errout;

	if (unlockpt(ptym_fd) < 0)
		goto errout;

	return ptym_fd;

errout:
	;
	/* close(2) can change errno */
	int err_saved = errno;
	close(ptym_fd);
	errno = err_saved;
	return -1;	
}

int ptys_open(const char *pts_name) {
	return open(pts_name, O_RDWR);
}

pid_t pty_fork(int *ptm_fd, char *pts_name, size_t pts_buff_len,
	       const struct termios *slave_termios, const struct winsize *slave_winsize) {

	if ((*ptm_fd = ptym_open(pts_name, pts_buff_len)) < 0)
		return -1;

	pid_t pid;
	int pipefds[2];
	int child_status;
	int err_code;

	if (pipe(pipefds) < 0) {
		err_code = errno;
		goto errout;
	}

	if ((pid = fork()) < 0) {
		err_code = errno;
		goto pipeout1;
	}

	if (pid == 0) {

		int pts_fd = 0;
		int status = 0;

		close(pipefds[0]);
		close(*ptm_fd);

		if (setsid() < 0)
			goto childout;

		if ((pts_fd = ptys_open(pts_name)) < 0)
			goto childout;

		if (slave_termios != NULL)
			if (tcsetattr(pts_fd, 0, slave_termios) < 0)
				goto childout;

		if (slave_winsize != NULL)
			if (ioctl(pts_fd, TIOCSWINSZ, slave_winsize) < 0)
				goto childout;

		if (dup2(pts_fd, STDIN_FILENO) < 0)
			goto childout;
		if (dup2(pts_fd, STDOUT_FILENO) < 0)
			goto childout;
		if (dup2(pts_fd, STDERR_FILENO) < 0)
			goto childout;

		write(pipefds[1], &status, sizeof(status));
		close(pipefds[1]);
		close(pts_fd);

		return 0;

	childout:
		status = errno;
		write(pipefds[1], &status, sizeof(status));
		exit(EXIT_FAILURE);
	}

	close(pipefds[1]);

	ssize_t n;
	if ((n = read(pipefds[0], &child_status, sizeof(child_status))) != sizeof(child_status)) {
		if (n >= 0)
			err_code = EIO;
		goto pipeout0;
	} else if (child_status != 0) {
		err_code = child_status;
		goto pipeout0;
	}

	close(pipefds[0]);

	return pid;

pipeout1:
	close(pipefds[1]);
pipeout0:
	close(pipefds[0]);
errout:
	close(*ptm_fd);
	errno = err_code;
	return -1;
}
