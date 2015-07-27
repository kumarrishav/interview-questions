
#include "pty_lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>
#include <fcntl.h>

/* pty accepts the following arguments:
 *
 * -d /path/to/driver
 *
 * Connect the PTY master's input to the driver's stdout and the PTY master's
 * output to the driver's stdin. This essentially enables the driver program
 * to control the terminal as if a user was typing input to the underlying
 * process. Typing input is akin to writing to stdout and reading terminal
 * output is equivalent to reading from stdin.
 *
 *
 * -e
 *
 * Disable ECHO on the pseudoterminal device.
 *
 *
 * -i
 * Ignore EOF when writing to the PTY master. The effect of this is that the
 * pty child (the pty child reads from stdin and writes to the PTY master) does
 * not notify the pty parent when EOF is reached on stdin. As such, the pty
 * master does not terminate and keeps writing the output from the PTY master.
 * This option is useful when running a long-living program for which we want
 * stdio to switch to line-buffered mode. We run the program with pty, redirect
 * the program's stdin to /dev/null, and use this flag to prevent the pty child
 * from killing the pty parent (since reading from /dev/null yields EOF). So
 * the child dies and the parent just keeps copying the other process's output
 * to stdout. This is an excellent way to get realtime, line-by-line output for
 * programs for which we don't have the source code (so we can't change stdio
 * buffering), since stdio will be running behind a terminal and enter
 * line-buffered mode.
 *
 *
 * -n
 * Disable interactive mode. In interactive mode, the current terminal is set
 * to raw mode with the help of cfmakeraw(3) and it is restored before pty
 * terminates. This option disables interactive mode, so the terminal settings
 * are untouched. Note that this option can lead to surprising results: the PTY
 * device has ECHO enabled by default, if we disable interactive mode on our
 * own terminal, then we have 2 terminals echoing, so we will see everything
 * twice.
 *
 *
 * -v
 * Verbose. Prints the slave PTY pathname after allocation.
 *
 */

#define OPTSTR "+d:einv"

static int set_noecho(int fd);
static int tty_raw(int fd);
static void tty_atexit(void);
static int do_driver(char *driver);
static int loop(int fd_master, int ignoreeof);
static ssize_t feed(int read_from_fd, int write_to_fd);
static void sigterm_hdr(int signo);

static struct termios termios_original;
static struct termios termios_raw;
static jmp_buf jmp_env;

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
		/* do_driver() does the necessary stdin and stdout redirections */
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

	struct sigaction sigact;
	sigact.sa_handler = sigterm_hdr;
	sigact.sa_flags = 0;
	sigemptyset(&sigact.sa_mask);

	if (sigaction(SIGTERM, &sigact, NULL) < 0)
		return -1;

	sigset_t smask, orig;
	sigemptyset(&smask);
	sigaddset(&smask, SIGTERM);
	if (sigprocmask(SIG_BLOCK, &smask, &orig) < 0)
		return -1;

	ssize_t n;
	pid_t pid;

	if ((pid = fork()) < 0) {
		return 0;
	}

	if (pid == 0) {

		/* Child reads from stdin and writes into pty master */

		sigact.sa_handler = SIG_DFL;
		if (sigaction(SIGTERM, &sigact, NULL) < 0) {
			perror("sigaction(2) error");
			exit(EXIT_FAILURE);
		}

		if (sigprocmask(SIG_SETMASK, &orig, NULL) < 0) {
			perror("sigprocmask(2) error");
			exit(EXIT_FAILURE);
		}

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

		/* It is important to understand why we use setjmp(3) and longjmp(3) here. The
		 * reason is race conditions. We could simply read from fd_master, knowing that
		 * read(2) would be interrupted when the child terminates because the child
		 * notifies us with SIGTERM.
		 *
		 * However, the child may terminate *before* the parent has the chance to call
		 * read(2). The parent would block forever in the call to read(2), since we catch
		 * SIGTERM and just set a flag that is tested after read(2). In fact, it's a little
		 * worse than that: we can never be sure that SIGTERM was sent before calling
		 * read(2).
		 *
		 * One could think that it would be enough to check if SIGTERM was caught before
		 * calling read(2), as in while (!sigcaught && read(...) > 0) { ... }, but this
		 * would only leave us on the same position: there is a window of time between
		 * testing the flag and calling read(2) where the signal might arrive (this is also
		 * known as a TOCTTOU error - Time Of Check To Time Of Use).
		 *
		 * Instead, we use setjmp(3) and longjmp(3) in combination with signal masks to
		 * avoid race conditions as follows:
		 *
		 * - Before forking, we install a signal handler for SIGTERM and block SIGTERM by
		 *   changing the process mask. This ensures that the parent does not receive
		 *   the signal before it even calls setjmp(3). It also ensures that the child
		 *   receives SIGTERM only after it has reset the signal disposition (since we don't
		 *   want to catch the signal in the child), even if the parent terminates before
		 *   the child has the chance to do so.
		 *
		 * - After forking, the child resets the signal disposition and only then it
		 *   unblocks SIGTERM, for the reasons mentioned above. Meanwhile, the parent calls
		 *   setjmp(3) to initialize the environment buffer. If setjmp(3) returns 0, meaning
		 *   we didn't jump, then it is safe to unblock SIGTERM, since the environment
		 *   buffer is now initialized. Note that the child may have terminated and sent
		 *   SIGTERM long ago, but the parent will only receive it after setjmp(3) was
		 *   called to initialize the buffer.
		 *
		 * - The signal handler installed for SIGTERM makes a non-local jump with longjmp(3)
		 *   when the child terminates. That will cause setjmp(3) to return a value that is
		 *   not 0. When that happens, we just return on the parent, to simulate a modem
		 *   hangup.
		 *
		 */

		if (setjmp(jmp_env) == 1) {
			/* EOF on pty child */
			return 0;
		} else {
			if (sigprocmask(SIG_SETMASK, &orig, NULL) < 0) {
				perror("sigsetmask(2) error when attempting to restore it");
				exit(EXIT_FAILURE);
			}
		}

		while ((n = feed(fd_master, STDOUT_FILENO)) > 0)
			; /* Intentionally left blank */

		kill(pid, SIGTERM);
	}

	/* Parent returns to caller */
	return n >= 0 ? 0 : -1;
}

static ssize_t feed(int read_from_fd, int write_to_fd) {

	static char data_buff[2048];
	ssize_t n, written;

	if ((n = read(read_from_fd, data_buff, sizeof(data_buff))) < 0) {

#ifdef linux
		/* Apparently in Linux read(2) returns -1 and sets errno to EIO when the slave pty
		 * device is closed and we are blocked waiting for data.
		 * If this is the case, we simply ignore the error
		 */
		return errno == EIO ? 0 : -1;
#endif /* linux */

		return -1;
	}

	if (n == 0)
		return 0;

	if ((written = write(write_to_fd, data_buff, n)) != n) {
		if (written >= 0)
			errno = EIO;
		return -1;
	}

	return n;
}

static void sigterm_hdr(int signo) {
	longjmp(jmp_env, 1);
}
