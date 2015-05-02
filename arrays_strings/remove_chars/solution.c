/* Write an efficient function that deletes characters from an ASCII string. Use the prototype
 * string removeChars(string str, string remove);
 *
 * where any character existing in `remove` must be deleted from `str`. For example,
 * given a `str` of "Battle of the Vowels: Hawaii vs. Grozny" and a `remove` of "aeiou",
 * the function should transform `str` to "Bttl f th Vwls: Hw vs. Grzny".
 * Justify any design decisions you make, and discuss the efficiency of your solution.
 *
 * Source: Programming Interviews Exposed, page 88
 */
#include <stdio.h>
#include <limits.h>
#include <string.h>
#define MAX_STR_SZ 256

void remove_chars(char *str, const char *remove) {
	static unsigned char rem_buf[UCHAR_MAX+1];

	if (str == NULL || remove == NULL || remove[0] == '\0' || str[0] == '\0') {
		return;
	}

	memset(rem_buf, 0, sizeof(rem_buf));

	size_t i;
	for (i = 0; remove[i] != '\0'; i++) {
		rem_buf[(unsigned char) remove[i]] = 1;
	}

	size_t j;
	for (i = 0, j = 0; str[i] != '\0'; i++) {
		if (rem_buf[(unsigned char) str[i]] == 0) {
			str[j++] = str[i];
		}
	}

	str[j] = '\0';
}


static char input_str[MAX_STR_SZ];
static char rem_str[MAX_STR_SZ];

int main(void) {
	printf("Enter input string, followed by the string of characters to remove\n");

	while (scanf("%s %s", input_str, rem_str) == 2) {
		printf("Before removal: %s\n", input_str);
		remove_chars(input_str, rem_str);
		printf("After removal: %s\n", input_str);
	}

	return 0;
}
