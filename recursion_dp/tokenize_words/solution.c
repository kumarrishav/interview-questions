/* You have a dictionary API that exports a single method, dict_find(), which allows you to check if
 * a word is stored in the dictionary.
 *
 * Given an input string with no spaces, segment the string into a space-separated sequence of
 * dictionary words if possible.
 *
 * EXAMPLE
 *
 * Input:  "applepie" (assume the dictionary contains the words "apple" and "pie", among others)
 * Output: "apple pie"
 *
 * Source: Careercup (Google interview)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int dict_find(const char *str, size_t str_sz);

int tokenize_words_aux(const char *str, size_t str_len, char *buffer) {
	if (*str == '\0')
		return 1;

	size_t i;
	for (i = 1; i <= str_len; i++) {
		if (!dict_find(str, i))
			continue;
		int written = sprintf(buffer, "%.*s ", (int) i, str);
		if (tokenize_words_aux(&str[i], str_len-i, buffer+written))
			return 1;
	}

	return 0;
}

char *tokenize_words(const char str[]) {
	if (str == NULL || *str == '\0')
		return NULL;

	size_t str_len = strlen(str);
	char *buffer;
	if ((buffer = malloc(2*str_len)) == NULL)
		return NULL;

	if (tokenize_words_aux(str, str_len, buffer)) {
		/* No trailing whitespace */
		size_t res_len = strlen(buffer);
		if (res_len > 0)
			buffer[res_len-1] = '\0';
		return buffer;
	}

	return NULL;
}

/* A naive but simple dictionary library */

static struct {
	char *word;
	size_t word_sz;	       
} dictionary[] = {
	{ "apple", sizeof("apple")-1 },
	{ "pie", sizeof("pie")-1 },
	{ "cat", sizeof("cat")-1 },
	{ "a", sizeof("a")-1 },
	{ "is", sizeof("is")-1 },
	{ "the", sizeof("the")-1 },
	{ "dinner", sizeof("dinner")-1 },
	{ "good", sizeof("good")-1 },
	{ "bad", sizeof("bad")-1 },
	{ "with", sizeof("with")-1 },
	{ "and", sizeof("and")-1 }
};

int dict_find(const char *str, size_t str_sz) {
	size_t i;
	for (i = 0; i < sizeof(dictionary)/sizeof(*dictionary); i++) {
		if (str_sz == dictionary[i].word_sz && !strncmp(str, dictionary[i].word, str_sz)) {
			return 1;
		}
	}
	return 0;
}

static char input_buff[1024];

int main(void) {
	printf("Enter a string to tokenize into words\n");
	printf("> ");

	while (scanf("%s", input_buff) == 1) {
		char *res = tokenize_words(input_buff);
		if (res == NULL)
			printf("Unsolvable\n");
		else
			printf("%s\n", res);
		printf("> ");
	}

	return 0;
}
