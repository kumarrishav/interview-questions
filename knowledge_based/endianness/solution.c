/* Write a C function that determines whether a computer is big-endian or little-endian
 *
 * Source: Programming Interviews Exposed, page 201
 */

#include <stdio.h>

int is_bigendian(void) {
	int a = 1;
	return *(char *)&a == 0;
}

int main(void) {
	if (is_bigendian()) {
		printf("System is big endian.\n");
	} else {
		printf("System is little endian.\n");
	}
	return 0;
}
