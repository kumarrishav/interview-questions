/* Given a string, write a function to check if it is a permutation of a palindrome.
 * A palindrome is a word or phrase that is the same forwards and backwards. A permutation
 * is a rearrangement of letters. The palindrome does not need to be limited to just dictionary
 * words.
 *
 * EXAMPLE
 *
 * Input: tactcoa
 * Output: true (permutations: "tacocat", "atcocta", etc.)
 *
 * Source: Cracking the Coding Interview (6th Ed.), page 91, exercise 1.4
 */

#include <stdio.h>
#include <string.h>
#define ALPHABET_SZ 26

/* Assumes string with characters in [a-z] only */
int palindrome_permutation(const char *str) {
	static char freq[ALPHABET_SZ];

	memset(freq, 0, sizeof(freq));

	size_t i;
	for (i = 0; str[i] != '\0'; i++) {
		size_t idx = (unsigned char) str[i]-'a';
		freq[idx] = (freq[idx]+1)%2;
	}

	int odd_occurrences = 0;
	for (i = 0; i < sizeof(freq) && odd_occurrences < 2; i++) {
		if (freq[i] == 1) {
			odd_occurrences++;
		}
	}

	return odd_occurrences < 2;
}

static char strbuff[1024];

int main(void) {
	printf("Enter a string\n");
	printf("> ");

	while (scanf("%s", strbuff) == 1) {
		if (palindrome_permutation(strbuff)) {
			printf("Yes\n");
		} else {
			printf("No\n");
		}
		printf("> ");
	}

	return 0;
}
