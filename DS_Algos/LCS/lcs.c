/* Longest Common Subsequence (LCS)
 * This file implements the LCS algorithm to determine and applies it to strings
 *
 * From Wikipedia:
 *
 * The longest common subsequence problem is a classic computer science problem, the basis of data
 * comparison programs such as the diff utility, and has applications in bioinformatics.
 * It is also widely used by revision control systems such as Git for reconciling multiple changes
 * made to a revision-controlled collection of files.
 *
 */
#include <stdio.h>
#include <string.h>
#include <assert.h>
#define max(a, b) ((a) > (b) ? (a) : (b))
#define MAX_STR_SZ 1024

static void reverse_str(char str[], size_t str_sz) {
	if (str_sz <= 1) {
		return;
	}

	size_t i = 0;
	size_t j = str_sz-1;
	while (i < j) {
		char tmp = str[i];
		str[i] = str[j];
		str[j] = tmp;
		i++;
		j--;
	}
}

static void build_lcs_str(const char *str1, size_t str1_sz, const char *str2, size_t str2_sz,
		   unsigned table[][MAX_STR_SZ+1], char *out) {
	size_t out_i = 0;
	size_t i = str1_sz;
	size_t j = str2_sz;

	while (i > 0 && j > 0) {
		if (str1[i-1] == str2[j-1]) {
			out[out_i++] = str1[i-1];
			i--;
			j--;
		} else if (table[i][j-1] > table[i-1][j]) {
			j--;
		} else {
			i--;
		}
	}

	out[out_i] = '\0';
	reverse_str(out, out_i);

}

size_t lcs(const char *str1, const char *str2, char *out) {

	size_t M = strlen(str1);
	size_t N = strlen(str2);

	assert(M <= MAX_STR_SZ);
	assert(N <= MAX_STR_SZ);

	static unsigned lcs_table[MAX_STR_SZ+1][MAX_STR_SZ+1];

	size_t i;
	for (i = 0; i < M; i++) {
		lcs_table[i][0] = 0;
	}
	for (i = 0; i < N; i++) {
		lcs_table[0][i] = 0;
	}

	for (i = 1; i < M+1; i++) {
		size_t j;
		for (j = 1; j < N+1; j++) {
			if (str1[i-1] == str2[j-1]) {
				lcs_table[i][j] = lcs_table[i-1][j-1]+1;
			} else {
				lcs_table[i][j] = max(max(lcs_table[i][j-1], lcs_table[i-1][j]),
						      lcs_table[i-1][j-1]);
			}
		}
	}

	build_lcs_str(str1, M, str2, N, lcs_table, out);

	return lcs_table[M][N];

}

static char input_str1[MAX_STR_SZ+1];
static char input_str2[MAX_STR_SZ+1];
static char lcs_result[MAX_STR_SZ+1];

int main(void) {
	printf("Enter two strings to find out the longest common subsequence\n");
	printf("> ");

	while (scanf("%s%s", input_str1, input_str2) == 2) {
		size_t lcs_sz = lcs(input_str1, input_str2, lcs_result);
		printf("LCS size = %zu\n", lcs_sz);
		if (lcs_sz > 0) {
			printf("%s\n", lcs_result);
		}
		printf("> ");
	}

	return 0;
}
