/* Describe the steps taken to daemonize a process, and write a function, daemonize(), that
 * does just that
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/fcntl.h>
#include <syslog.h>
#include <unistd.h>
#include <signal.h>

#define DAEMON_NAME "my_daemon"

/* This implementation is based on what is described in
 * Advanced Programming in the UNIX Environment, 3rd ed.
 * (Chapter 13)
 *
 * Example execution:
 *
 * filipe@filipe-Kubuntu:~$ ./a.out
 * Started my_daemon
 * filipe@filipe-Kubuntu:~$ ps aux | grep a.out
 * filipe   25407  102  0.0   4328   436 ?        R    15:58   0:34 ./a.out
 * filipe@filipe-Kubuntu:~$ kill -SIGHUP 25407
 * filipe@filipe-Kubuntu:~$
 *
 * Messages on syslog:
 *
 * May 29 15:58:58 filipe-Kubuntu my_daemon: daemonize() complete. PID = 25407
 * May 29 15:59:14 filipe-Kubuntu my_daemon: Got SIGHUP, rehashing...
 *
 */

void sighup_handler(int signo) {
	syslog(LOG_INFO, "Got SIGHUP, rehashing...\n");
	/* ... */
}

void terminate_handler(int signo) {
	syslog(LOG_INFO, "Terminating...\n");
	exit(EXIT_SUCCESS);
}

int daemonize(void) {

	umask(0);

	pid_t pid;
	pid = fork();

	if (pid < 0) {
		return -1;
	}

	/* Ensure that we're not a process group leader,
	 * so that it's safe to create a new session
	 */
	if (pid != 0) {
		exit(0);
	}

	setsid();

	/* Close every open file descriptor */
	struct rlimit rlim;
	int max_fd;
	if (getrlimit(RLIMIT_NOFILE, &rlim) < 0 || rlim.rlim_cur == RLIM_INFINITY) {
		max_fd = 1024;
	} else {
		max_fd = rlim.rlim_cur;
	}

	int i;
	for (i = 0; i < max_fd; i++) {
		close(i);
	}

	/* chdir to root, or to a specified, known location where the daemon
	 * will do its work
	 */
	chdir("/");

	/* Prepare error / message logging */
	openlog(DAEMON_NAME, 0, 0);

	/* Ensure that we're not a session leader to prevent controlling terminal allocation
	 * This is recommended on System V based systems, but not really needed on other UNIX
	 * flavors
	 */
	pid = fork();
	if (pid < 0) {
		syslog(LOG_ERR, "Couldn't fork() after creating a new session.\n");
		exit(EXIT_FAILURE);
	}
	if (pid != 0) {
		exit(0);
	}

	/* Some library functions and system utilities expect to have stdin, stdout and stderr
	 * opened */
	int sink = open("/dev/null", O_RDWR, 0);
	dup2(sink, STDIN_FILENO);

	if (sink != STDIN_FILENO) {
		close(sink);
	}

	dup2(STDIN_FILENO, STDOUT_FILENO);
	dup2(STDIN_FILENO, STDERR_FILENO);

	/* Catch SIGHUP. SIGHUP on daemons is typically used to force the daemon to reread its
	 * configuration file.
	 * Why SIGHUP? Because SIGHUP is the signal sent by the terminal driver to the controlling
	 * process in a session when the terminal driver detects a network disconnect or a modem
	 * hangup (hence "HUP").
	 * Since daemons do not have a controlling terminal, SIGHUP can't possibly be triggered
	 * by a terminal disconnect. Thus, people have historically used it with a different meaning
	 * in daemons
	 */

	struct sigaction sighandler;
	sighandler.sa_handler = sighup_handler;
	sighandler.sa_flags = 0;
	sigemptyset(&sighandler.sa_mask);

	if (sigaction(SIGHUP, &sighandler, NULL) < 0) {
		syslog(LOG_ERR, "Couldn't catch SIGHUP.\n");
		exit(EXIT_FAILURE);
	}

	sighandler.sa_handler = terminate_handler;
	if (sigaction(SIGINT, &sighandler, NULL) < 0) {
		syslog(LOG_ERR, "Couldn't catch SIGINT.\n");
		exit(EXIT_FAILURE);
	}

	if (sigaction(SIGTERM, &sighandler, NULL) < 0) {
		syslog(LOG_ERR, "Couldn't catch SIGTERM.\n");
		exit(EXIT_FAILURE);
	}

	syslog(LOG_INFO, "daemonize() complete. PID = %ld\n", (long) getpid());
	syslog(LOG_INFO, "Send SIGTERM or SIGINT to terminate\n");

	return 0;
}

int main(void) {
	printf("Started %s\n", DAEMON_NAME);
	daemonize();
	
	/* What happens here? O.o
	 *
	 * MAC OS X seems to preserve the login name even in a daemonized process
	 * (tested on Yosemite)
	 *
	 * Linux, on the other hand, returns with errno set to ENOTTY, which makes a lot
	 * more sense
	 */
	char *login_name = getlogin();
	if (login_name == NULL) {
		syslog(LOG_INFO, "getlogin(3) error: %s\n", strerror(errno));
	} else {
		syslog(LOG_INFO, "getlogin(3) returned %s\n", login_name);
	}

	while (1) {
		sigset_t mask;
		sigemptyset(&mask);
		sigsuspend(&mask);
	}

	return 0;
}
