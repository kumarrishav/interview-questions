
#include "pty_lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>

#define OPTSTR "+d:einv"

static int set_noecho(int fd);
static int tty_raw(int fd);
static void tty_atexit(void);
static int do_driver(char *driver);
static int loop(int fd_master, int ignoreeof);
static ssize_t feed(int read_from_fd, int write_to_fd);

static struct termios termios_original;
static struct termios termios_raw;

int main(int argc, char *argv[]) {

	char *driver = NULL;
	int noecho = 0;
	int interactive = isatty(STDIN_FILENO);
	int verbose = 0;
	int ignoreeof = 0;

	int opt;
	while ((opt = getopt(argc, argv, OPTSTR)) != EOF) {
		switch (opt) {
		case 'd':
			driver = optarg;
			break;
		case 'e':
			noecho = 1;
			break;
		case 'i':
			ignoreeof = 1;
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

	if (driver != NULL) {
		if (do_driver(driver) < 0) {
			perror("do_driver() failed");
			exit(EXIT_FAILURE);
		}
	}


	int ret = loop(fd_master, ignoreeof);

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

static int do_driver(char *driver) {

	int pty_to_driver[2];
	int driver_to_pty[2];
	int err_code;

	if (pipe(pty_to_driver) < 0)
		return -1;

	if (pipe(driver_to_pty) < 0) {
		err_code = errno;
		goto ptdout;
	}

	pid_t pid;
	if ((pid = fork()) < 0) {
		err_code = errno;
		goto dtpout;
	}

	if (pid == 0) {
		if (close(pty_to_driver[1]) < 0) {
			perror("Error closing pty_to_driver write channel on driver child");
			exit(EXIT_FAILURE);
		}
		if (close(driver_to_pty[0]) < 0) {
			perror("Error closing driver_to_pty read channel on driver child");
			exit(EXIT_FAILURE);
		}
		if (dup2(pty_to_driver[0], STDIN_FILENO) < 0) {
			perror("Error duplicating pty_to_driver read channel to stdin on driver child");
			exit(EXIT_FAILURE);
		}
		if (pty_to_driver[0] != STDIN_FILENO) {
			if (close(pty_to_driver[0]) < 0) {
				perror("Error closing pty_to_driver read channel after successfully duplicating it to stdin");
				exit(EXIT_FAILURE);
			}
		}
		if (dup2(driver_to_pty[1], STDOUT_FILENO) < 0) {
			perror("Error duplicating driver_to_pty write channel to stdout on driver child");
			exit(EXIT_FAILURE);
		}
		if (driver_to_pty[1] != STDOUT_FILENO) {
			if (close(driver_to_pty[1]) < 0) {
				perror("Error closing driver_to_pty write channel after successfully duplicating it to stdout");
				exit(EXIT_FAILURE);
			}
		}
		if (execlp(driver, driver, NULL) < 0) {
			perror("execl(2) error");
			exit(EXIT_FAILURE);
		}
	}

	if (close(driver_to_pty[1]) < 0) {
		err_code = errno;
		goto dtpout;
	}
	if (close(pty_to_driver[0]) < 0) {
		err_code = errno;
		goto dtpout;
	}
	if (dup2(pty_to_driver[1], STDOUT_FILENO) < 0) {
		err_code = errno;
		goto dtpout;
	}
	if (pty_to_driver[1] != STDOUT_FILENO) {
		if (close(pty_to_driver[1]) < 0) {
			err_code = errno;
			goto dtpout;
		}
	}
	if (dup2(driver_to_pty[0], STDIN_FILENO) < 0) {
		err_code = errno;
		goto dtpout;
	}
	if (driver_to_pty[0] != STDIN_FILENO) {
		if (close(driver_to_pty[0]) < 0) {
			err_code = errno;
			goto dtpout;
		}
	}

	return 0;

dtpout:
	close(driver_to_pty[0]);
	close(driver_to_pty[1]);
ptdout:
	close(pty_to_driver[0]);
	close(pty_to_driver[1]);
	errno = err_code;
	return -1;
}

static int loop(int fd_master, int ignoreeof) {
	ssize_t n;
	pid_t pid;

	if ((pid = fork()) < 0) {
		return 0;
	}

	if (pid == 0) {
		/* Child reads from stdin and writes into pty master */
		while ((n = feed(STDIN_FILENO, fd_master)) > 0)
			; /* Intentionally left blank */

		if (n < 0)
			perror("feed() error on pty child");

		/* We only notify the parent if ignoreeof is not set; but the
		 * parent always notifies us upon termination.
		 *
		 * This is useful if stdin is redirected to something like
		 * /dev/null but the program is a long-living application
		 * that produces output over time.
		 */
		if (!ignoreeof)
			kill(getppid(), SIGTERM);

		/* Child terminates here */
		exit(n < 0 ? EXIT_FAILURE : EXIT_SUCCESS);

	} else {
		/* Parent reads from pty master and writes to stdout */
		while ((n = feed(fd_master, STDOUT_FILENO)) > 0)
			; /* Intentionally left blank */

		/* Child is always notified; ignoreeof only affects
		 * child -> parent notifications
		 */
		kill(pid, SIGTERM);
	}

	/* Parent returns to caller */
	return n >= 0 ? 0 : -1;
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
