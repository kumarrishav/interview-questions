/* Write a program that checks whether stdin is seekable */

#include <unistd.h>
#include <stdio.h>

/* Sockets, pipes and FIFOs are not seekable. If stdin was redirected to any of
 * these file types, an error will be returned by lseek().
 *
 * So, this is successful:
 *
 * ./a.out </etc/passwd
 *
 * But this is not:
 *
 * cat /etc/passwd | ./a.out
 *
 */
int main(void) {
	off_t stdin_offset;
	stdin_offset = lseek(STDIN_FILENO, 0, SEEK_CUR);
	if (stdin_offset == -1) {
		perror("stdin is not seekable");
	} else {
		printf("stdin is seekable\n");
	}
	return 0;
}
