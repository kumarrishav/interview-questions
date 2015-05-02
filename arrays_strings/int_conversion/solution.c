/* Write two conversion routines. The first routine converts a string
 * to a signed integer. You may assume that the string contains only digits and the
 * minus character ('-'), that it is a properly formatted integer number, and that the
 * number is within the range of an `int` type. The second routine converts a signed integer
 * stored as an `int` back to a string.
 *
 * Source: Programming Interviews Exposed, page 87
 */

#include <stdio.h>

// Assumption: 0-9 are consecutive on the platform's encoding
int char_to_digit(char c) {
	return c - '0';
}

char digit_to_char(int d) {
	return d + '0';
}

// Assumption: str is well formatted and represents an `int` that is not out of range
int str_to_int(const char *str) {
	size_t curr = 0;

	int res = 0;
	int sign = 1;

	if (str[curr] == '-') {
		sign = -1;
		curr++;
	}

	while (str[curr] != '\0') {
		res = res*10+char_to_digit(str[curr]);
		curr++;
	}

	return res*sign;
}

void reverse_str(char *, char *);
void int_to_str(int n, char *res) {

	if (n == 0) {
		*res++ = '0';
		*res = '\0';
		return;
	}

	size_t i = 0;
	int sign = 1;

	if (n < 0) {
		sign = -1;
	}

	while (n != 0) {
		res[i++] = digit_to_char(sign*(n%10));
		n /= 10;
	}

	if (sign == -1) {
		res[i++] = '-';
	}

	res[i] = '\0';
	reverse_str(res, res+i);
}

void reverse_str(char *str, char *end) {
	if (str == end) {
		return;
	}

	end--;

	while (str < end) {
		char tmp = *str;
		*str = *end;
		*end = tmp;
		str++;
		end--;
	}
}

static char input[32];

int main(void) {
	printf("Enter a valid integer as a string\n");

	while (scanf("%s", input) == 1) {
		printf("Received: %s\n", input);
		int converted = str_to_int(input);
		printf("`char *` -> `int`: %d\n", converted);
		int_to_str(converted, input);
		printf("`int` -> `char *`: %s\n", input);
	}

	return 0;
}
