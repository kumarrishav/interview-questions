/* Assume you have a method isSubstring which checks if one word is a
 * substring of another. Given two strings, s1 and s2, write code to check if s2 is
 * a rotation of s1 using only one call to isSubstring (e.g. "waterbottle" is a
 * rotation of "erbottlewat").
 *
 * Source: Cracking the Coding Interview, page 74, question 1.8
 */
#include <stdio.h>
#include <string.h>

int is_substring(const char *haystack, const char *needle) {
	return strstr(haystack, needle) != NULL;
}

int is_rotation(const char *s1, const char *s2) {
	size_t s1_len = strlen(s1);
	char buff[s1_len*2+1];
	strcpy(buff, s1);
	strcpy(buff+s1_len, s1);
	return is_substring(buff, s2);
}

static char input_s1[256];
static char input_s2[256];

int main(void) {
	printf("Enter s1 followed by s2\n");

	while (scanf("%s%s", input_s1, input_s2) == 2) {
		printf("%s is a rotation of %s: %s\n", input_s1, input_s2,
		       is_rotation(input_s1, input_s2) ? "yes" : "no");
	}

	return 0;
}
