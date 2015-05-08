/* FOLLOW UP
 * Reimplement PrintTelephoneWords without using recursion.
 *
 * Source: Programming Interviews Exposed, page 122
 */
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#define NUMBER_LEN 7

/* Note:
 * This function does not accurately emulate a phone terminal for keys >= 7
 * This is because 7 maps to PRS, and the function assumes that each key >= 2
 * maps to the 3 next letters in the alphabet (so, it assumes 7 maps to PQR,
 * 8 to STU, and 9 to VWX).
 * Thus, for keys >= 7, it returns the wrong letters. But this is not really relevant
 * for the algorithm. I just implemented it this way for convenience, and because it
 * makes it easy to come up with a general formula, instead of hardcoding everything.
 */
char get_char_key(int telephone_key, int place) {
	assert(2 <= telephone_key && telephone_key <= 9);
	assert(1 <= place && place <= 3);
	return 'a'+(telephone_key-2)*3+(place-1);
}

static int increment(int places[]) {
	int i;
	for (i = NUMBER_LEN-1; i >= 0; i--) {
		if (places[i] == -1) {
			continue;
		}
		if (places[i] != 3) {
			break;
		}
		places[i] = 1;
	}

	if (i == -1) {
		return 0;
	}

	places[i]++;
	return 1;
}

static void print_number(int places[], int number[]) {
	size_t i;
	for (i = 0; i < NUMBER_LEN; i++) {
		if (places[i] == -1) {
			putchar(number[i]+'0');
		} else {
			putchar(get_char_key(number[i], places[i]));
		}
	}
	putchar('\n');
}

void print_words(int number[]) {
	int places[NUMBER_LEN];

	size_t i;
	for (i = 0; i < NUMBER_LEN; i++) {
		if (number[i] == 0 || number[i] == 1) {
			places[i] = -1;
		} else {
			places[i] = 1;
		}
	}

	print_number(places, number);
	while (increment(places)) {
		print_number(places, number);
	}
}

static int read_number(int number[]) {
	size_t i;
	int c;

	while (isspace(c = getchar()));
	ungetc(c, stdin);

	for (i = 0; i < NUMBER_LEN; i++) {
		c = getchar();
		if (!('0' <= c && c <= '9')) {
			return 0;
		}
		number[i] = c-'0';
	}
	return 1;
}

int main(void) {

	printf("Enter 7 consecutive digits representing a US phone number (without area code)\n");
	printf("> ");

	int phone_number[NUMBER_LEN];
	while (read_number(phone_number)) {
		print_words(phone_number);
		printf("> ");
	}

	return 0;
}
