/* Imagine a robot sitting on the upper left corner of an X by Y grid. The robot can
 * only move in two directions: right and down. How many possible paths are there
 * for the robot to go from (0,0) to (X,Y)?
 *
 * Source: Cracking the Coding Interview, page 109, question 9.2
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* Naive recursion */
unsigned long long paths_aux(size_t curr_x, size_t curr_y, size_t x, size_t y) {
	unsigned long long res = 0;

	if (curr_x == x && curr_y == y) {
		return 1;
	}

	if (curr_x < x) {
		res += paths_aux(curr_x+1, curr_y, x, y);
	}

	if (curr_y < y) {
		res += paths_aux(curr_x, curr_y+1, x, y);
	}

	return res;
}

unsigned long long paths_rec(size_t x, size_t y) {
	return paths_aux(0, 0, x, y);
}

/* DP solution */
unsigned long long paths_dp(size_t x, size_t y) {
	unsigned long long **dp = malloc(sizeof(*dp)*(x+1));
	assert(dp != NULL);

	size_t i;
	for (i = 0; i < x+1; i++) {
		dp[i] = malloc(sizeof(*dp[i])*(y+1));
		assert(dp[i] != NULL);
	}

	for (i = 0; i < x+1; i++)
		dp[i][0] = 1;
	for (i = 0; i < y+1; i++)
		dp[0][i] = 1;

	size_t j;
	for (i = 1; i < x+1; i++)
		for (j = 1; j < y+1; j++)
			dp[i][j] = dp[i][j-1]+dp[i-1][j];

	unsigned long long ret = dp[x][y];

	for (i = 0; i < x+1; i++)
		free(dp[i]);
	free(dp);

	return ret;
}

int main(void) {
	size_t x, y;

	printf("Enter the values for X and Y\n> ");

	while (scanf("%zu%zu", &x, &y) == 2) {

		// printf("Naive recursion: %llu\n", paths_rec(x, y));
		printf("DP: %llu\n> ", paths_dp(x, y));
	}

	return 0;
}
