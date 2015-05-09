
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


/* ~~~ Question (b) ~~~ */
int score(const char *word);

char *find_best_word_aux(char **board, size_t board_dims, ssize_t row, ssize_t col,
			 char *word_buff, size_t buff_sz, size_t buff_i, int *best_score) {
	if (row < 0 || row == board_dims || col < 0 || col == board_dims) {
		*best_score = -1;
		return NULL;
	}

	if (buff_i == buff_sz-1) {
		word_buff[buff_i] = '\0';
		*best_score = score(word_buff);
		if (*best_score  == -1) {
			return NULL;
		} else {
			char *best_word = strdup(word_buff);
			assert(best_word != NULL);
			return best_word;
		}
	}

	word_buff[buff_i] = board[row][col];
	word_buff[buff_i+1] = '\0';

	int best_so_far = score(word_buff);
	if (best_so_far == -1) {
		*best_score = -1;
		return NULL;
	}

	char *best_word = strdup(word_buff);
	assert(best_word != NULL);

	ssize_t i;
	for (i = -1; i < 2; i++) {
		ssize_t j;
		for (j = -1; j < 2; j++) {
			if (i == 0 && j == 0) {
				continue;
			}
			int score;
			char *w;

			w = find_best_word_aux(board, board_dims, row+i, col+j,
					       word_buff, buff_sz, buff_i+1, &score);

			if (score > best_so_far) {
				best_so_far = score;
				free(best_word);
				best_word = w;
			} else {
				free(w);
			}
		}
	}

	*best_score = best_so_far;
	return best_word;
}

char *find_best_word(char **board, size_t board_dim) {
	static char word_buffer[1024];
	int score;
	return find_best_word_aux(board, board_dim, 0, 0,
				  word_buffer, sizeof(word_buffer), 0, &score);
}
/* ~~~ End of question (b) ~~~ */

/* Dummy score function */
int score(const char *word) {
	if (!strcmp(word, "e") ||
	    !strcmp(word, "en") ||
	    !strcmp(word, "eng") ||
	    !strcmp(word, "engi") ||
	    !strcmp(word, "engin") ||
	    !strcmp(word, "engine") ||
	    !strcmp(word, "enginee") ||
	    !strcmp(word, "engineer") ||
	    !strcmp(word, "engineeri") ||
	    !strcmp(word, "engineerin") ||
	    !strcmp(word, "engineering")) {
		return strlen(word);
	}

	return -1;
}

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

		printf("Best word: ");
		char *word = find_best_word(board, n);
		if (word == NULL) {
			printf("no words\n");
		} else {
			printf("%s\n", word);
		}
		free(word);

		destroy_board(board, n);
		printf("> ");
	}

	return 0;
}
