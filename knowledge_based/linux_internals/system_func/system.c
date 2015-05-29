/* Describe how system() is implemented */

#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/wait.h>
#include <stdlib.h>

/* POSIX compliance requires that system() blocks SIGCHLD and ignores SIGQUIT
 * and SIGINT on the current process (note that the child process that executes
 * the shell does not obey this requirement)
 *
 * The rationale behind this decision is that in this context it doesn't make
 * sense for the current process (the parent) to catch terminal-generated
 * signals from arbitrary commands being executed in system(). If system()
 * is invoked with an interactive program that catches SIGQUIT or SIGINT,
 * both the process running that program and the parent process in system()
 * will catch the signal (because they are on the same process group), and the
 * parent will mistakenly execute signal handlers that may not make sense,
 * since the signal wasn't targeted at the parent in the first place.
 * Similarly, SIGQUIT is ignored for the same reasons.
 *
 * SIGCHLD is blocked because we don't want the parent to receive SIGCHLD
 * for the termination of the child process created in system() - again,
 * because the parent process may have installed signal handlers for SIGCHLD
 * and is expecting that the signal is raised only for the child processes
 * that have been created in the normal execution flow. The parent may not be
 * aware that system() forked a child process, and forcing user code to
 * deal with this implementation detail would be a bad design choice.
 *
 * Note that SIGCHLD is blocked, not ignored, because we don't want to lose
 * track of the other "real" SIGCHLD signals that come from genuine
 * child processes.
 * 
 * Also, POSIX states that if wait() or waitpid() returns the status
 * of a child process while SIGCHLD is pending, then SIGCHLD should not
 * be delivered to the process unless another child process's exit status
 * is also available. This is why SIGCHLD is blocked inside system().
 * Sadly, Linux doesn't follow this rule, and ends up delivering the signal
 * anyway, but if we call wait() or waitpid() inside the signal handler
 * for system()'s child process ID, wait() returns -1 with errno
 * set to ECHILD, because system() has already acknowledged that child's death.
 *
 */

int system(const char *cmd) {

	if (cmd == NULL) {
		// Assuming UNIX environment
		return 1;
	}

	struct sigaction ign, saveint, savequit;
	sigset_t block_chld, savemask;

	ign.sa_handler = SIG_IGN;
	sigemptyset(&ign.sa_mask);
	ign.sa_flags = 0;

	if (sigaction(SIGQUIT, &ign, &savequit) < 0) {
		return -1;
	}
	if (sigaction(SIGINT, &ign, &saveint) < 0) {
		sigaction(SIGQUIT, &savequit, NULL);
		return -1;
	}

	sigemptyset(&block_chld);
	sigprocmask(SIG_BLOCK, &block_chld, &savemask);

	int ret;
	pid_t f = fork();

	if (f < 0) {
		ret = -1;
	} else if (f == 0) {
		sigaction(SIGQUIT, &savequit, NULL);
		sigaction(SIGINT, &saveint, NULL);
		sigprocmask(SIG_SETMASK, &savemask, NULL);
		execl("/bin/sh", "sh", "-c", cmd, (char *) 0);
		exit(127);
	} else {
		while (waitpid(f, &ret, 0) < 0) {
			if (errno != EINTR) {
				ret = -1;
				break;
			}
		}
	}

	sigaction(SIGINT, &saveint, NULL);
	sigaction(SIGQUIT, &savequit, NULL);
	sigprocmask(SIG_SETMASK, &savemask, NULL);

	return ret;
}

int main(void) {
	printf("Entered main()\n");
	system("ls -al");
	printf("Back to main.\n");
	return 0;
}
