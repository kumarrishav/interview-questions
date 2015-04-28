/* Write a method to replacee all spaces in a string with '%20'. You may assume that
 * the string has sufficient space at the end of the string to hold the additional
 * characters, and that you are given the "true" length of the string. (Note: if
 * implementing in Java, please use a character array so that you can perform this
 * operation in place).
 * EXAMPLE
 * Input:  "Mr John Smith"
 * Output: "Mr%20John%20Smith"
 *
 * Source: Cracking the Coding Interview, page 73, question 1.4
 */

#include <stdio.h>

char *escape_spaces(char *str) {
	if (str == NULL) {
		return NULL;
	}

	size_t new_len = 0;
	size_t i;
	for (i = 0; str[i] != '\0'; i++) {
		if (str[i] == ' ') {
			new_len += 3;
		} else {
			new_len++;
		}
	}

	if (new_len == 0 || new_len == i) {
		return str;
	}

	str[new_len] = '\0';
	size_t j = new_len-1;
	for (i--; i > 0; i--) {
		if (str[i] == ' ') {
			str[j--] = '0';
			str[j--] = '2';
			str[j--] = '%';
		} else {
			str[j--] = str[i];
		}
	}

	if (str[i] == ' ') {
		str[j--] = '0';
		str[j--] = '2';
		str[j--] = '%';
	} else {
		str[j--] = str[i];
	}

	return str;
}

static char buff[1024];

int main(void) {
	printf("Enter a string to escape spaces\n");

	while (scanf("%[^\n]s", buff) == 1) {
		printf("Received:\t%s\n", buff);
		printf("Escaped:\t%s\n", escape_spaces(buff));
		getchar(); // Consume newline
	}

	return 0;
}
