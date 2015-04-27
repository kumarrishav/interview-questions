/* Implement an algorithm to determine if a string has all unique characters.
 * What if you cannot use additional data structures?
 *
 * Source: Cracking the Coding Interview, page 73, question 1.1
 */

#include <stdio.h>
#include <limits.h>

int has_unique_chars(char *str) {
	size_t chars_count[UCHAR_MAX] = { 0 };

	if (str == NULL) {
		return 1;
	}

	for (; *str != '\0'; str++) {
		if (++chars_count[(unsigned char) *str] > 1) {
			return 0;
		}
	}

	return 1;
}

int has_unique_chars_no_buff(char *str) {
	if (str == NULL) {
		return 1;
	}

	size_t i;
	for (i = 0; str[i] != '\0'; i++) {
		size_t j;
		for (j = i+1; str[j] != '\0'; j++) {
			if (str[i] == str[j]) {
				return 0;
			}
		}
	}

	return 1;
}

static char buff[1024];
int main(void) {
	printf("Enter a string to check if it has all unique chars\n");

	while (scanf("%s", buff) == 1) {
		printf("Has all unique chars, with buffer: %s\n",
		       has_unique_chars(buff) ? "yes" : "no");
		printf("Has all unique chars, without buffer: %s\n",
		       has_unique_chars_no_buff(buff) ? "yes" : "no");
	}

	return 0;
}
