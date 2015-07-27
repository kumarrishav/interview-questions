
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <termios.h>
#include <unistd.h>
#include <poll.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

static struct termios original, raw;

int loop(int sockfd);
int handle_event(struct pollfd *pfd, int sockfd);
int prepare_terminal(void);
void tty_atexit(void);

int main(int argc, char *argv[]) {
	if (argc < 3) {
		fprintf(stderr, "Usage: %s ip port\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	uint16_t port;
	if (sscanf(argv[2], "%" SCNu16, &port) != 1) {
		fprintf(stderr, "Invalid port number: %s\n", argv[2]);
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in sockaddr;
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(port);

	int s = inet_pton(AF_INET, argv[1], &sockaddr.sin_addr);
	if (s <= 0) {
		if (s == 0) {
			fprintf(stderr, "Invalid address: %s\n", argv[1]);
		} else {
			perror("Internal error on inet_pton(3)");
		}
		exit(EXIT_FAILURE);
	}

	int sockfd;
	if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("socket(2) failed");
		exit(EXIT_FAILURE);
	}

	if (connect(sockfd, (const struct sockaddr *) &sockaddr, sizeof(sockaddr)) < 0) {
		perror("connect(2) failed");
		exit(EXIT_FAILURE);
	}

	if (prepare_terminal() < 0) {
		perror("prepare_terminal() error");
		exit(EXIT_FAILURE);
	}

	if (loop(sockfd) < 0) {
		perror("loop() error");
		exit(EXIT_FAILURE);
	}

	return 0;
}

int loop(int sockfd) {
	struct pollfd pollfds[2];

	pollfds[0].fd = STDIN_FILENO;
	pollfds[0].events = POLLIN;

	pollfds[1].fd = sockfd;
	pollfds[1].events = POLLIN;

	while (poll(pollfds, sizeof(pollfds)/sizeof(pollfds[0]), -1) > 0) {
		if ((pollfds[0].revents & (POLLERR | POLLHUP)) ||
		    (pollfds[1].revents & (POLLERR | POLLHUP)))
			return 0;
		if (handle_event(&pollfds[0], sockfd) < 0)
			return -1;
		if (handle_event(&pollfds[1], sockfd) < 0)
			return -1;
	}

	return -1;
}

int handle_event(struct pollfd *pfd, int sockfd) {

	static char buff[1024];

	if (pfd->revents == 0)
		return 0;

	if (pfd->revents != POLLIN)
		return -1;

	int fd_from = pfd->fd;
	int fd_to = (fd_from == STDIN_FILENO ? sockfd : STDOUT_FILENO);
	ssize_t n;

	if ((n = read(fd_from, buff, sizeof(buff))) < 0)
		return -1;

	ssize_t written;
	if ((written = write(fd_to, buff, n)) != n) {
		if (written >= 0)
			errno = EIO;
		return -1;
	}

	return 0;
}

int prepare_terminal(void) {

	if (tcgetattr(STDIN_FILENO, &original) < 0)
		return -1;

	if (atexit(tty_atexit) < 0)
		return -1;

	cfmakeraw(&raw);

	if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) < 0)
		return -1;

	return 0;
}

void tty_atexit(void) {
	tcsetattr(STDIN_FILENO, TCSANOW, &original);
}
