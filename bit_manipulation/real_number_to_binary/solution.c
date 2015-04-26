/* Given a real number between 0 and 1 (e.g., 0.72) that is passed in as a double,
 * print the binary representation. If the number cannot be represented accurately
 * in binary with at most 32 characters, print "ERROR".
 *
 * Source: Cracking the Coding Interview, page 92, question 5.2
 */

#include <stdio.h>

void print_binary_rep(double n) {
	static size_t max_bits = 32;
	// \0 Terminator + "0."
	char buff[max_bits+3];

	size_t cursor = 0;
	cursor += sprintf(buff, "0.");

	while (n != 0 && cursor < 34) {
		double new_n = n*2;
		int new_n_as_int = new_n;
		buff[cursor++] = (new_n_as_int == 1 ? '1' : '0');
		n = new_n-new_n_as_int;
	}

	if (n != 0) {
		sprintf(buff, "ERROR");
	} else {
		buff[cursor] = '\0';
	}

	printf("%s\n", buff);
}

int main(void) {
	printf("Enter a real number in the range ]0, 1[\n");

	double n;
	while (scanf("%lf", &n) == 1) {
		print_binary_rep(n);
	}

	return 0;
}
