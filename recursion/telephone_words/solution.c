
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#define NUMBER_LEN 7

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
		if (c == EOF) {
			return 0;
		}
		number[i] = c-'0';
	}
	return 1;
}

int main(void) {

	printf("Enter 7 digits representing a US phone number (without area code)\n");
	printf("> ");

	int phone_number[NUMBER_LEN];
	while (read_number(phone_number)) {
		print_words(phone_number);
		printf("> ");
	}

	return 0;
}
