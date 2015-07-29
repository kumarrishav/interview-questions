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

/* The code below implements a naive recursive solution that basically tests all possible splitting
 * points in the string and returns as soon as it finds a combination that consumes the entire
 * string.
 *
 * What is the time complexity of such a solution? One possible way to derive it is to see how the
 * recursion tree grows: the first layer branches n times (because we can split the string in any
 * of n positions). In the second layer, the first node will branch n-1 times, the second will
 * branch n-2 times, the third will branch n-3, etc.
 *
 * Given that the depth of the recursion tree is O(n), and each node never branches more than O(n)
 * times, the running time is bounded by O(n^n), which is horrible.
 *
 * Note that the actual runtime is not as bad as O(n^n), because as we go deeper in the tree, each
 * node will branch less and less, so it may be too agressive to assume an O(n) branching factor.
 *
 * A tighter bound can be found with a different analysis. We can see that the recursive solution is
 * essentially generating every possible splitting combination. It's as if we look at the string as
 * a sequence of indexes, and we choose a group of indexes that are the splitting positions. We can
 * pick 0 indexes, 1 index, 2, 3, 4, ...
 *
 * In other words, the recursion is generating the powerset of N elements (the powerset of all
 * indexes), and so it is easy to see that the running time is actually O(2^N). Not exactly
 * efficient, but it's way better than O(n^n).
 *
 * Can we do better? Yes. This is a good candidate problem for dynamic programming, because there is
 * a chance that we call the tokenize function with the same suffix more than once. So we can use
 * memoization to remember that a given suffix didn't work, saving us the hassle of doing all of the
 * recursive work again.
 * Note that the algorithm stops as soon as a solution is found, so we really just cache the suffix
 * combinations that did NOT work in the past (so, in the code below we would do that in the line
 * before returning 0).
 *
 * If we look at the recursion tree, we can see how much time this buys us. Usually, deriving the
 * running time of a memoized function is easy if we think bottom-up. How do we build a solution
 * for a node out of the branched nodes? If we were to do it bottom-up, we would have a DP table
 * of the same size as the input string, where dp[i] represents the solution for the suffix i.
 *
 * Building dp[i-1] requires iterating through all of the suffixes of the i-th suffix, so in a
 * bottom-up solution, we would have a nested for loop, something like:
 *
 * // dp[i] = " " means no solution found for this suffix
 * for (i = 0; i < len; i++)
 *     dp[i] = " ";
 *
 * dp[len] = "";
 * 
 * for (i = len-1; i >= 0; i--) {
 *     // Add new solutions that appear with the new letter
 *     for (j = i+1; j < len+1; j++) {
 *         if (dict_find(str[i..j-1]) && dp[j] != " ") {
 *             // New solution found: the word str[i..j-1] with the solution in dp[j]
 *             dp[i] += str[i..j-1] + " " + dp[j];
 *         }
 *     }
 * }
 *
 * This is pseudocode, but anyway, the point is, we can easily see that it will be O(n^2), a
 * significant (huge!) improvement over the O(2^N) naive solution.
 *
 * The bottom-up approach also has the added benefit of avoiding stack overflow altogether, which
 * is wonderful! We get O(n) space and O(n^2) time. Nice!
 *
 * See solution_dp.cpp for the implementation of this bottom-up approach.
 *
 */

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
