/* Implement a function that prints all possible combinations of the
 * characters in a string. These combinations range in length from one to the length
 * of the string. Two combinations that differ only in ordering of their characters are
 * the same combination. In other words, "12" and "31" are different combinations
 * from the input string "123", but "21" is the same as "12".
 */
#include <stdio.h>
#include <string.h>
#define ALPHABET_SZ 26

/* Note:
 * The implementation assumes that the string can have duplicate characters.
 * It never generates a duplicated combination even if the string contains duplicate
 * characters.
 * To achieve that, first it sorts the string as a preprocessing step.
 * It is assumed that the strings only contain lower-case letters from the English
 * alphabet, and therefore sorting is done in O(n).
 */

static void print_combinations_aux(unsigned char letters[], size_t letter_i,
			    char *buff, size_t buff_i) {
	size_t next_letter;
	for (next_letter = letter_i; next_letter < ALPHABET_SZ && !letters[next_letter]; next_letter++)
		; /* Intentionally left blank */

	if (next_letter == ALPHABET_SZ) {
		buff[buff_i] = '\0';
		// We don't want the empty subset
		if (buff_i != 0) {
			printf("%s\n", buff);
		}
		return;
	}

	print_combinations_aux(letters, next_letter+1, buff, buff_i);
	buff[buff_i] = 'a'+next_letter;
	print_combinations_aux(letters, next_letter+1, buff, buff_i+1);
}

void print_combinations(const char *str) {
	static unsigned char letters[ALPHABET_SZ];

	memset(letters, 0, sizeof(letters));

	// Sort the string in O(n) - assumes bounded alphabet
	size_t i;
	for (i = 0; str[i] != '\0'; i++) {
		letters[(unsigned char) (str[i]-'a')] = 1;
	}

	char buff[i+1];
	print_combinations_aux(letters, 0, buff, 0);
}

static char input_buff[1024];
int main(void) {
	printf("Enter a string to get its powerset.\n");
	printf("> ");

	while (scanf("%s", input_buff) == 1) {
		print_combinations(input_buff);
		printf("> ");
	}

	return 0;
}
