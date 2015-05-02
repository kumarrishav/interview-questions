/* Write a function that reverses the order of the words in a string. For
 * example, your function should transform the string "Do or do not, there is no try."
 * to "try. no is there not, do or Do". Assume that all words are space delimited
 * and treat punctuation the same as letters.
 *
 * Source: Programming Interviews Exposed, page 87
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <stdlib.h>
#define INPUT_BUFF_SZ 256

size_t next_word_end(const char *str) {
	size_t res = 0;
	while (str[res] != '\0' && !isspace((unsigned char) str[res])) {
		res++;
	}

	return res;
}


/* Approach 1: Recursive solution. A good exercise to play with and study recursion.
 * O(n) time and O(n) memory
 */
size_t reverse_words_aux(char *str, size_t curr);
void reverse_words_1(char *str) {
	(void) reverse_words_aux(str, 0);
}

size_t reverse_words_aux(char *str, size_t curr) {
	size_t next_end = next_word_end(&str[curr]);
	if (str[curr+next_end] == '\0') {
		return sprintf(str, "%s", &str[curr]);
	}

	char *word = strndup(&str[curr], next_end);
	assert(word != NULL);

	curr = reverse_words_aux(str, curr+next_end+1);

	size_t ret = curr+sprintf(str+curr, " %s", word);
	free(word);

	return ret;
}

/* Approach 2: O(1) memory, O(n) time */
void reverse_str(char *str, char *end);
void reverse_words_2(char *str) {
	size_t next_end = 0;
	size_t curr = 0;

	while (str[curr] != '\0') {
		next_end = next_word_end(&str[curr]);
		reverse_str(&str[curr], &str[curr+next_end]);
		curr += next_end;
		if (str[curr] != '\0') {
			curr++;
		}
	}

	reverse_str(str, &str[curr]);
}

void reverse_str(char *str, char *end) {
	if (str == end) {
		return;
	}

	end--;

	while (str < end) {
		char tmp = *str;
		*str = *end;
		*end = tmp;
		str++;
		end--;
	}
}

static char input_str[INPUT_BUFF_SZ];
static char input_str_copy[INPUT_BUFF_SZ];
int main(void) {
	printf("Enter a string with words separated by spaces. "
	       "It cannot start or end with spaces.\n");

	while (scanf(" %[^\n]s", input_str) == 1) {
		strcpy(input_str_copy, input_str);
		printf("Received: %s\n", input_str);
		reverse_words_1(input_str);
		printf("Recursive reverse: %s\n", input_str);
		reverse_words_2(input_str_copy);
		printf("Rotation-based reverse: %s\n", input_str_copy);
	}

	return 0;
}
