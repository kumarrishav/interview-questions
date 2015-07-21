#ifndef PTY_LIB_H
#define PTY_LIB_H

#include <stddef.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

int posix_openpt(int flags);
int ptym_open(char *pts_name, size_t buff_len);
int ptys_open(const char *pts_name);
pid_t pty_fork(int *ptm_fd, char *pts_name, size_t pts_buff_len,
	       const struct termios *slave_termios, const struct winsize *slave_winsize);

#endif /* PTY_LIB_H */
