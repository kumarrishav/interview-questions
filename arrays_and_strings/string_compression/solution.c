/* Implement a method to perform basic string compression using the counts
 * of repeated characters. For example, the string aabcccccaaa would become
 * a2b1c5a3. If the "compressed" string would not become smaller than the original
 * string, your method should return the original string.
 *
 * Source: Cracking the Coding Interview, page 73, question 1.5
 */
#include <stdio.h>
#include <string.h>

char *compress_str(char *str) {
	if (str == NULL) {
		return NULL;
	}

	size_t original_sz = strlen(str);
	if (original_sz < 2) {
		return str;
	}

	// Worst case: each char is replaced by 2 characters
	// e.g. ab -> a1b1
	char buff[original_sz*2+1];
	size_t buff_i = 0;

	size_t i = 0;
	while (str[i] != '\0') {
		size_t char_count = 1;
		size_t j = i+1;
		while (str[j] != '\0' && str[j] == str[i]) {
			char_count++;
			j++;
		}
		buff_i += sprintf(buff+buff_i, "%c%zu", str[i], char_count);
		i = j;
	}

	if (!(buff_i < original_sz)) {
		return str;
	} else {
		return strcpy(str, buff);
	}
}

static char buffer[1024];

int main(void) {
	printf("Enter a string to compress\n");

	while (scanf("%s", buffer) == 1) {
		printf("Received:\t%s\n", buffer);
		printf("Compressed:\t%s\n", compress_str(buffer));
	}

	return 0;
}
