
#include "pty_lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <sys/epoll.h>

#define OPTSTR "+d:env"

static int set_noecho(int fd);
static int tty_raw(int fd);
static void tty_atexit(void);
static int loop(int fd_master);

static struct termios termios_original;
static struct termios termios_raw;

int main(int argc, char *argv[]) {

	char *driver = NULL;
	int noecho = 0;
	int interactive = isatty(STDIN_FILENO);
	int verbose = 0;

	int opt;
	while ((opt = getopt(argc, argv, OPTSTR)) != EOF) {
		switch (opt) {
		case 'd':
			driver = optarg;
			break;
		case 'e':
			noecho = 1;
			break;
		case 'n':
			interactive = 0;
			break;
		case 'v':
			verbose = 1;
			break;
		case '?':
			fprintf(stderr, "Unrecognized option: -%c\n", optopt);
			exit(EXIT_FAILURE);
		}
	}

	if (optind >= argc) {
		fprintf(stderr, "Usage: %s [ -d driver -env ] program [ arg ... ]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	struct termios *tmios = NULL;
	struct winsize *winsz = NULL;

	struct termios orig_termios;
	struct winsize size;

	if (interactive) {
		if (tcgetattr(STDIN_FILENO, &orig_termios) < 0) {
			perror("tcgetattr(2) failed in interactive mode");
			exit(EXIT_FAILURE);
		}
		if (ioctl(STDIN_FILENO, TIOCGWINSZ, (char *) &size) < 0) {
			perror("ioctl(2) failed in interactive mode when attempting to get window size");
			exit(EXIT_FAILURE);
		}
		tmios = &orig_termios;
		winsz = &size;
	}

	pid_t pid;
	int fd_master;
	char pts_buff[64];

	if ((pid = pty_fork(&fd_master, pts_buff, sizeof(pts_buff), tmios, winsz)) < 0) {
		perror("pty_fork() failed");
		exit(EXIT_FAILURE);
	}

	if (pid == 0) {
		if (noecho && set_noecho(STDIN_FILENO) < 0) {
			perror("set_noecho() failed");
			exit(EXIT_FAILURE);
		}
		if (execvp(argv[optind], &argv[optind]) < 0) {
			perror("execvp(2) error on child");
			exit(EXIT_FAILURE);
		}
	}

	if (verbose) {
		fprintf(stderr, "slave name = %s\n", pts_buff);
		if (driver != NULL)
			fprintf(stderr, "driver = %s\n", driver);
	}

	if (interactive && driver == NULL) {
		if (tty_raw(STDIN_FILENO) < 0) {
			perror("tty_raw() failed");
			exit(EXIT_FAILURE);
		}
		atexit(tty_atexit);
	}
/*
	if (driver)
		do_driver(driver);
*/

	int ret = loop(fd_master);

	if (ret < 0)
		perror("Error on main loop");

	return ret;
}

static int set_noecho(int fd) {
	struct termios stermios;
	if (tcgetattr(fd, &stermios) < 0)
		return -1;

	stermios.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL);
	stermios.c_oflag &= ~(ONLCR);

	return tcsetattr(fd, TCSANOW, &stermios);
}

static int tty_raw(int fd) {
	if (tcgetattr(fd, &termios_original) < 0)
		return -1;
	termios_raw = termios_original;
	cfmakeraw(&termios_raw);
	return tcsetattr(fd, 0, &termios_raw);
}

static void tty_atexit(void) {
	tcsetattr(STDIN_FILENO, 0, &termios_original);
}

static ssize_t feed(int read_from_fd, int write_to_fd);

/* Main loop, connects PTY to stdin and stdout */
static int loop(int fd_master) {
	int epoll_fd;

	if ((epoll_fd = epoll_create(1)) < 0)
		return -1;

	struct epoll_event epevent;

	epevent.events = EPOLLIN;
	epevent.data.fd = STDIN_FILENO;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &epevent) < 0)
		goto errout;

	epevent.data.fd = fd_master;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd_master, &epevent) < 0)
		goto errout;

	struct epoll_event events[2];
	int monitoring = 2;

	while (monitoring > 0) {
		int n;
		if ((n = epoll_wait(epoll_fd, events, sizeof(events), -1)) < 0)
			goto errout;
		int i;
		for (i = 0; i < n; i++) {
			if (events[i].events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)) {
				/* This should never fail */
				if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, NULL) < 0) {
					fprintf(stderr,
						"Internal error: failed to remove %d from epoll set: %s\n",
						events[i].data.fd, strerror(errno));
				}
				monitoring--;
			} else if (events[i].events & EPOLLIN) {
				int from = events[i].data.fd;
				int to = (from == STDIN_FILENO ? fd_master : STDOUT_FILENO);
				ssize_t fed;

				if ((fed = feed(from, to)) < 0)
					goto errout;

				if (fed == 0) {
					if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, from, NULL) < 0) {
						fprintf(stderr,
							"Internal error: failed to remove %d after read(2) returned 0: %s\n",
							from, strerror(errno));
					}
				}
			} else {
				fprintf(stderr, "Internal error: unexpected event caught on main loop; fd = %d, events = %" PRIu32 "\n",
					events[i].data.fd, events[i].events);
			}
		}
	}

	return 0;

errout:
	;
	int saved_err = errno;
	close(epoll_fd);
	errno = saved_err;
	return -1;
}


static ssize_t feed(int read_from_fd, int write_to_fd) {

	static char data_buff[2048];
	ssize_t n, written;

	if ((n = read(read_from_fd, data_buff, sizeof(data_buff))) < 0)
		return -1;

	if (n == 0)
		return 0;

	if ((written = write(write_to_fd, data_buff, n)) != n) {
		if (n >= 0)
			errno = EIO;
		return -1;
	}

	return n;
}
