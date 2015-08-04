/* There are two strings, A and B, such that A's length is greater than or equal to B's length.
 * Write a function that finds all of the offsets in A where a permutation of B begins.
 *
 * EXAMPLE
 *
 * Input:
 * A = mississippi
 * B = sis
 *
 * Output: 1 2 3 4 5
 *
 * Explanation:
 * A[1] is the beginning of a permutation of "sis" (the permutation is "iss")
 * A[2] is the beginning of a permutation of "sis" (the permutation is "ssi")
 * Etc...
 *
 * Source: Careercup (Microsoft interview)
 */

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <limits.h>

/* The following solution solves the problem in linear time, or, to be more precise, O(N+M), where
 * N and M is the length of each string. Assume the largest string has size N, and the shortest has
 * size M. A naive approach is to iterate through each index i in the large string, compute the
 * character frequency table of large_str[i..i+M-1], and compare it to the short string's character
 * frequency table - if they are the same, then we report i as an index.
 *
 * However, computing the character frequency table of each substring from scratch and comparing it
 * against the short string's table is O(M), so this would be O(NM).
 *
 * We can improve on this and make it O(N+M). First, compute the character frequency table for the
 * short string. Then, compute the character frequency table for large_str[0..M-1]. As we build it
 * for the large string, we keep track of how many unique positions were filled, and how many of
 * those matched with positions in the short string frequency table.
 *
 * Then it works as if there was a sliding window on the large string. When we move it right one
 * place, we remove the character on the left, and add the new character on the right, updating the
 * number of matched positions so far, along with the number of total positions.
 *
 * It takes some care to implement it correctly, but this added complexity allows us to know how
 * many positions have been matched so far and how many unique positions are set in the character
 * frequency table. So we can test if both tables match in O(1): they are the same if the number of
 * matched positions on the large table is equal to the number of positions filled in the small
 * table, and if the number of total filled positions in the large stable is equal to the number of
 * positions filled in the small table.
 */

void match_permutations_aux(const char *big_str, const char *short_str, size_t short_len,
			    const size_t short_freq[UCHAR_MAX+1], size_t nfreq) {

	static size_t big_freq[UCHAR_MAX+1];
	memset(big_freq, 0, sizeof(big_freq));

	size_t total_big = 0;
	size_t matched = 0;
	size_t i;
	for (i = 0; i < short_len; i++) {
		unsigned char idx = big_str[i];
		if (big_freq[idx] == short_freq[idx] && big_freq[idx] != 0)
			matched--;

		if (big_freq[idx] == 0)
			total_big++;

		big_freq[idx]++;

		if (big_freq[idx] == short_freq[idx])
			matched++;
	}

	if (matched == nfreq && total_big == nfreq)
		printf("0 ");

	for (i = short_len; big_str[i] != '\0'; i++) {
		unsigned char idx_remove = big_str[i-short_len];
		unsigned char idx_add = big_str[i];

		assert(big_freq[idx_remove] > 0);

		/* Remove leftmost character */
		if (big_freq[idx_remove] == short_freq[idx_remove])
			matched--;
		big_freq[idx_remove]--;
		if (big_freq[idx_remove] == short_freq[idx_remove] && big_freq[idx_remove] != 0)
			matched++;
		if (big_freq[idx_remove] == 0)
			total_big--;

		/* Add rightmost character */
		if (big_freq[idx_add] == 0)
			total_big++;
		if (big_freq[idx_add] == short_freq[idx_add] && big_freq[idx_add] != 0)
			matched--;
		big_freq[idx_add]++;
		if (big_freq[idx_add] == short_freq[idx_add])
			matched++;

		if (matched == nfreq && total_big == nfreq)
			printf("%zu ", i-short_len+1);

	}
}

void match_permutations(const char *str_a, const char *str_b) {
	if (str_a == NULL || str_b == NULL)
		return;

	const char *big_str = str_a;
	const char *short_str = str_b;
	size_t short_len;

	if (strlen(str_b) > strlen(str_a)) {
		big_str = str_b;
		short_str = str_a;
	}

	short_len = strlen(short_str);

	static size_t short_freq[UCHAR_MAX+1];
	memset(short_freq, 0, sizeof(short_freq));

	size_t total_short = 0;
	size_t i;
	for (i = 0; i < short_len; i++) {
		if (short_freq[(unsigned char) short_str[i]] == 0)
			total_short++;
		short_freq[(unsigned char) short_str[i]]++;
	}

	match_permutations_aux(big_str, short_str, short_len, short_freq, total_short);
	putchar('\n');
}

static char str_a_buff[1024];
static char str_b_buff[1024];

int main(void) {
	printf("Enter string A and B.\n");
	printf("> ");

	while (scanf("%s%s", str_a_buff, str_b_buff) == 2) {
		match_permutations(str_a_buff, str_b_buff);
		printf("> ");
	}

	return 0;
}
