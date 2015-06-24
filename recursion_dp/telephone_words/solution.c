/* People in the United States often give others their telephone number
 * as a word representing the seven-digit number after the area code. For
 * example, if my telephone number were 866-2665, I could tell people my number
 * is "TOOCOOL", instead of the hard-to-remember seven-digit number. Note
 * that many other possibilities (most of which are nonsensical) can represent
 * 866-2665. You can see how letters correspond to numbers on a telephone keypad in
 * Figure 7-2.
 *
 *  ~~~ ~~~ ~~~
 * | 1 | 2 | 3 |
 * |   |ABC|DEF|
 *  ~~~ ~~~ ~~~
 * | 4 | 5 | 6 |
 * |GHI|JKL|MNO|
 *  ~~~ ~~~ ~~~
 * | 7 | 8 | 9 |
 * |PRS|TUV|WXY|
 *  ~~~ ~~~ ~~~
 * | 4 | 5 | 6 |
 * | * | 0 | # |
 *  ~~~ ~~~ ~~~
 *
 * Write a function that takes a seven-digit telephone number and prints out all of
 * the possible "words" or combinations of letters that can represent the given number.
 * Because the 0 and 1 keys have no letters on them, you should change only the digits
 * 2-9 to letters. You'll be passed an array of seven integers, with each element
 * being one digit in the number. You may assume that only valid phone numbers
 * will be passed to your function. You can use the helper function
 *
 * char getCharKey(int telephoneKey, int place)
 *
 * which takes a telephone key (0-9) and a place of either 1, 2, 3 and returns the
 * character corresponding to the letter in that position on the specified key. For
 * example, GetCharKey(3, 2) will return 'E' because the telephone key 3 has the
 * letters "DEF" on it and 'E' is the second letter.
 *
 * Source: Programming Interviews Exposed, page 119
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

static void print_words_aux(int number[], size_t number_i, char *buff, size_t buff_i) {
	if (number_i == NUMBER_LEN) {
		buff[buff_i] = '\0';
		printf("%s\n", buff);
		return;
	}

	if (number[number_i] < 2) {
		buff[buff_i] = '0'+number[number_i];
		print_words_aux(number, number_i+1, buff, buff_i+1);
	} else {
		size_t i;
		for (i = 1; i <= 3; i++) {
			buff[buff_i] = get_char_key(number[number_i], i);
			print_words_aux(number, number_i+1, buff, buff_i+1);
		}
	}
}

void print_words(int telephone[]) {
	char buff[NUMBER_LEN+1];
	print_words_aux(telephone, 0, buff, 0);
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
