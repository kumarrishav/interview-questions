/* Write a bare-bones implementation of the linux cp command.
 * Assume the file to copy is read from stdin and the target is stdout
 */
#include <stdio.h>

int main(void) {
	int c;
	while ((c = getchar()) != EOF) {
		putchar(c);
	}
	return 0;
}
