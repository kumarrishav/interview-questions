/* Boggle is a word game where players try to find words in an NxN board of letters. Each word
 * has a score.
 * As an example, consider this boggle board configuration:
 *
 *      (0) (1) (2) (3)
 *      ~~~ ~~~ ~~~ ~~~
 * (0) | K | G | W | I |
 *      ~~~ ~~~ ~~~ ~~~
 * (1) | N | I | R | G |
 *      ~~~ ~~~ ~~~ ~~~
 * (2) | E | L | N | E |
 *      ~~~ ~~~ ~~~ ~~~
 * (3) | S | O | A | E |
 *      ~~~ ~~~ ~~~ ~~~
 *
 * Among others, we can find the word "engineering"
 * (2,0 -> 1,0 -> 0,1 -> 1,1 -> 2,2 -> 3,3 -> 2,3 -> 1,2 -> 1,1 -> 1,0 -> 0,1)
 * and "song" (3,0 -> 3,1 -> 2,2 -> 1,3)
 *
 * Question (a):
 * Write a function that generates a random NxN boggle board ensuring that no letter is used
 * more than N times.
 *
 * Question (b):
 * Given a function `int score(char *)` that returns the score of a word, and an NxN boggle board,
 * find the word in the boggle board that has the maximum score.
 * score() will return -1 if the word is invalid.
 * If a word is invalid, you may assume that every word with that prefix is also invalid.
 *
 * FOLLOW UP
 * How would you store and lookup a word's score?
 *
 * Source: past interview experience
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#define ALPHABET_SZ 26
#define MAX_WORD_SZ 512

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

/* Follow up question
 * A good approach is to use a trie to efficiently find words and their score
 * Implementing a trie was not part of the interview - it was enough to mention the
 * data structure and explain how it can be used in this problem
 */

/* ~~~ Trie implementation to store words and scores ~~~ */
struct trie_node {
	int score; // -1 means not a word
	unsigned children_count;
	struct trie_node *children[ALPHABET_SZ];
};

static struct trie_node *new_node(void) {
	struct trie_node *ret = malloc(sizeof(*ret));
	if (ret == NULL) {
		return NULL;
	}

	ret->score = -1;
	ret->children_count = 0;
	memset(ret->children, 0, sizeof(ret->children));

	return ret;
}

struct trie_node *new_trie(void) {
	return new_node();
}

void destroy_trie(struct trie_node *root) {
	if (root == NULL) {
		return;
	}

	size_t i;
	for (i = 0; i < ALPHABET_SZ; i++) {
		destroy_trie(root->children[i]);
	}

	free(root);
}

void insert_word(struct trie_node *root, const char *word, int score) {
	size_t word_sz = strlen(word);
	size_t word_i = 0;
	struct trie_node *curr_node = root;

	while (word_i < word_sz &&
	       curr_node->children[(unsigned char) (word[word_i]-'a')] != NULL) {
		curr_node = curr_node->children[(unsigned char) (word[word_i]-'a')];
		word_i++;
	}

	if (word_i == word_sz) {
		// Update the score
		curr_node->score = score;
		return;
	}

	while (word_i < word_sz) {
		struct trie_node *n = new_node();
		assert(n != NULL);
		curr_node->children[(unsigned char) (word[word_i]-'a')] = n;
		curr_node->children_count++;
		curr_node = n;
		word_i++;
	}

	curr_node->score = score;
}

static int delete_word_aux(struct trie_node *root, const char *word, size_t word_i) {
	if (word[word_i] == '\0') {
		if (root->score != -1) {
			root->score = -1;
			return 1;
		} else {
			return 0;
		}
	}

	int index = (unsigned char) (word[word_i]-'a');

	if (root->children[index] == NULL) {
		return 0;
	}

	int ret = delete_word_aux(root->children[index], word, word_i+1);
	if (ret && root->children[index]->children_count == 0 &&
	    root->children[index]->score != -1) {
		free(root->children[index]);
		root->children[index] = NULL;
		root->children_count--;
	}

	return ret;
}

void delete_word(struct trie_node *root, const char *word) {
	delete_word_aux(root, word, 0);
}

int word_score(struct trie_node *root, const char *word) {
	struct trie_node *curr_node = root;

	while (*word != '\0' && curr_node->children[(unsigned char) (*word-'a')] != NULL) {
		curr_node = curr_node->children[(unsigned char) (*word-'a')];
		word++;
	}

	return (curr_node == NULL || *word != '\0') ? -1 : curr_node->score;
}


static void print_words(struct trie_node *root, size_t buff_i) {
	static char curr_word[MAX_WORD_SZ];

	if (root == NULL) {
		return;
	}

	if (root->score != -1) {
		curr_word[buff_i] = '\0';
		printf("%s -> %d\n", curr_word, root->score);
	}

	size_t i;
	for (i = 0; i < ALPHABET_SZ; i++) {
		curr_word[buff_i] = 'a'+i;
		print_words(root->children[i], buff_i+1);
	}
}

void print_known_words(struct trie_node *root) {
	print_words(root, 0);
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


static char word_buff[MAX_WORD_SZ];
static struct trie_node *trie;

int score(const char *word) {
	return word_score(trie, word);
}

int main(void) {
	srand(time(NULL));

	printf("The available commands are:\n"
	       "I N - Inject an NxN board. This command will scan NxN letters to form a board\n"
	       "G N - Generates a new NxN board where a letter doesn't appear more than N times\n"
	       "P - Print the current board\n"
	       "W N word - Insert a word into the dictionary with score N\n"
	       "A N word - Insert a word and all prefixes in the dictionary. Non-proper prefixes get a score of 0\n"
	       "R word - Delete a word\n"
	       "S word - Search for a word and return its score (-1 if no such word exists)\n"
	       "D - Dump the dictionary with the corresponding scores\n"
	       "B - Find the best word (word with the highest score)\n"
	       "Q - Quit\n"
	       "> ");

	char **board = NULL;
	size_t board_dim = 0;
	trie = new_trie();

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
		} else if (op == 'W') {
			size_t word_score;
			scanf("%zu%s", &word_score, word_buff);
			insert_word(trie, word_buff, word_score);
		} else if (op == 'A') {
			size_t word_score;
			scanf("%zu%s", &word_score, word_buff);

			size_t i;
			for (i = 1; word_buff[i] != '\0'; i++) {
				char c = word_buff[i];
				word_buff[i] = '\0';
				insert_word(trie, word_buff, 0);
				word_buff[i] = c;
			}
			insert_word(trie, word_buff, word_score);

		} else if (op == 'R') {
			scanf("%s", word_buff);
			delete_word(trie, word_buff);
		} else if (op == 'S') {
			scanf("%s", word_buff);
			int s = word_score(trie, word_buff);
			if (s == -1) {
				printf("No such word: %s\n", word_buff);
			} else {
				printf("score(%s) = %d\n", word_buff, s);
			}
		} else if (op == 'D') {
			print_known_words(trie);
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

	destroy_trie(trie);
	destroy_board(board, board_dim);

	return 0;
}
