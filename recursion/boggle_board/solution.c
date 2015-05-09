
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#define ALPHABET_SZ 26

/* ~~~ Question (a) ~~~ */
static char get_rand_letter(void) {
	return rand()%26+'a';
}

char **generate_board(size_t n) {
	unsigned letters_used[ALPHABET_SZ];
	memset(letters_used, 0, sizeof(letters_used));

	char **board = malloc(n*sizeof(*board));
	assert(board != NULL);

	size_t i;
	for (i = 0; i < n; i++) {
		board[i] = malloc(n*sizeof(board[i]));
		assert(board[i] != NULL);
	}

	for (i = 0; i < n; i++) {
		size_t j;
		for (j = 0; j < n; j++) {
			char letter = get_rand_letter();
			while (letters_used[(unsigned char) (letter-'a')] > n) {
				letter = get_rand_letter();
			}
			letters_used[(unsigned char) (letter-'a')]++;
			board[i][j] = letter;
		}
	}

	return board;
}

void destroy_board(char **board, size_t n) {

	size_t i;
	for (i = 0; i < n; i++) {
		free(board[i]);
	}

	free(board);
}
/* ~~~ End of Question (a) ~~~ */

static void print_board(char **board, size_t n) {
	size_t i;
	for (i = 0; i < n; i++) {
		size_t j;
		for (j = 0; j < n; j++) {
			putchar(board[i][j]);
		}
		putchar('\n');
	}
}



int main(void) {

	srand(time(NULL));

	size_t n;
	printf("Enter a size for the board.\n");
	printf("> ");

	while (scanf("%zu", &n) == 1) {
		char **board = generate_board(n);
		print_board(board, n);
		destroy_board(board, n);
		printf("> ");
	}

	return 0;
}
