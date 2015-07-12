/* A child is running up a staircase with n steps, and can hop either 1 step, 2 steps,
 * or 3 steps at a time. Implement a method to count how many possible ways the
 * child can run up the stairs.
 *
 * Source: Cracking the Coding Interview, page 109, question 9.1
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

unsigned long long possibilities_naive(unsigned n) {
	if (n == 0)
		return 1;
	unsigned long long res = possibilities_naive(n-1);
	if (n > 1)
		res += possibilities_naive(n-2);
	if (n > 2)
		res += possibilities_naive(n-3);
	return res;
}

unsigned long long possibilities(unsigned n) {
	unsigned long long *dp = malloc(sizeof(*dp)*(n+1));
	assert(dp != NULL);

	if (n >= 0)
		dp[0] = 1;
	if (n >= 1)
		dp[1] = dp[0];
	if (n >= 2)
		dp[2] = dp[1]+dp[0];

	unsigned i;
	for (i = 3; i < n+1; i++)
		dp[i] = dp[i-1]+dp[i-2]+dp[i-3];

	unsigned long long ret = dp[n];
	free(dp);
	return ret;
}

int main(void) {
	printf("Enter number of stairs\n");

	int stairs;
	while (scanf("%d", &stairs) == 1) {
		/* This is super slow
		 * printf("Naive: %llu\n", possibilities_naive(stairs));
		 */
		printf("DP: %llu\n", possibilities(stairs));
		printf("> ");
	}

	return 0;
}
