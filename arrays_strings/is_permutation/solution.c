/* Given two strings, write a method to decide if one is a permutation of the other.
 *
 * Source: Cracking the Coding Interview, page 73, question 1.3
 */
#include <stdio.h>
#include <string.h>
#include <limits.h>

int is_permutation(const char *str1, const char *str2) {
	int char_counts[UCHAR_MAX] = { 0 };

	size_t str1_len = strlen(str1);
	size_t str2_len = strlen(str2);

	if (str1_len != str2_len) {
		return 0;
	}

	size_t i;
	for (i = 0; i < str1_len; i++) {
		char_counts[(unsigned char) str1[i]]++;
	}

	for (i = 0; i < str2_len; i++) {
		if (--char_counts[(unsigned char) str2[i]] < 0) {
			return 0;
		}
	}

	return 1;
}

static char buff1[1024];
static char buff2[1024];

int main(void) {
	printf("Enter two strings\n");

	while (scanf("%s %s", buff1, buff2) == 2) {
		printf("Is permutation: %s\n", is_permutation(buff1, buff2) ? "yes" : "no");
	}

	return 0;
}
