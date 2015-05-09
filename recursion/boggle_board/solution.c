
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

static char inject_letter(void) {
	char l;
	scanf(" %c", &l);
	return l;
}

char **generate_board_aux(size_t n, char (*letter_gen)(void)) {
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
			char letter = (*letter_gen)();
			while (letters_used[(unsigned char) (letter-'a')] > n) {
				letter = (*letter_gen)();
			}
			letters_used[(unsigned char) (letter-'a')]++;
			board[i][j] = letter;
		}
	}

	return board;
}

char **generate_board(size_t n) {
	return generate_board_aux(n, get_rand_letter);
}

char **inject_board(size_t n) {
	return generate_board_aux(n, inject_letter);
}

void destroy_board(char **board, size_t n) {

	if (board == NULL) {
		return;
	}

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
	int best_score = -1;
	char *word = NULL;

	ssize_t i, j;
	for (i = 0; i < board_dim; i++) {
		for (j = 0; j < board_dim; j++) {
			int score;
			char *w = find_best_word_aux(board, board_dim, i, j,
						     word_buffer, sizeof(word_buffer), 0,
						     &score);
			if (score > best_score) {
				free(word);
				word = w;
				best_score = score;
			} else {
				free(w);
			}
		}
	}

	return word;
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
	    !strcmp(word, "engineering") ||
	    !strcmp(word, "s") ||
	    !strcmp(word, "so") ||
	    !strcmp(word, "son") ||
	    !strcmp(word, "song")) {
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

	printf("The available commands are:\n"
	       "I N - Inject an NxN board. This command will scan NxN letters to form a board\n"
	       "G N - Generates a new NxN board where a letter doesn't appear more than N times\n"
	       "P - Print the current board\n"
	       "B - Find the best word (word with the highest score)\n"
	       "Q - Quit\n"
	       "> ");

	char **board = NULL;
	size_t board_dim = 0;

	char op;
	while (scanf(" %c", &op) == 1) {
		if (op == 'I') {
			destroy_board(board, board_dim);
			scanf("%zu", &board_dim);
			board = inject_board(board_dim);
		} else if (op == 'P') {
			if (board == NULL) {
				printf("No board at the moment\n");
			} else {
				print_board(board, board_dim);
			}
		} else if (op == 'G') {
			destroy_board(board, board_dim);
			scanf("%zu", &board_dim);
			board = generate_board(board_dim);
		} else if (op == 'B') {
			char *w = find_best_word(board, board_dim);
			if (w == NULL) {
				printf("No words found\n");
			} else {
				printf("Best word: %s\n", w);
			}
			free(w);
		} else if (op == 'Q') {
			break;
		} else {
			fprintf(stderr, "Unrecognized operation: %c\n", op);
		}

		printf("> ");
	}

	destroy_board(board, board_dim);

	return 0;
}
