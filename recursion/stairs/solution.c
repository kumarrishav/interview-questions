/* A child is running up a staircase with n steps, and can hop either 1 step, 2 steps,
 * or 3 steps at a time. Implement a method to count how many possible ways the
 * child can run up the stairs.
 *
 * Source: Cracking the Coding Interview, page 109, question 9.1
 */
#include <stdio.h>
#include <string.h>

unsigned long long possibilities_aux(int left, unsigned long long cached[]) {
	if (left < 0) {
		return 0;
	}
	if (left == 0) {
		return 1;
	}
	if (cached[left-1] != 0) {
		return cached[left-1];
	} else {
		cached[left-1] =
			possibilities_aux(left-1, cached) +
			possibilities_aux(left-2, cached) +
			possibilities_aux(left-3, cached);
	}

	return cached[left-1];
}

unsigned long long possibilities(int stairs) {
	unsigned long long cached[stairs];
	memset(cached, 0, sizeof(cached));
	return possibilities_aux(stairs, cached);
}

int main(void) {
	printf("Enter number of stairs\n");

	int stairs;
	while (scanf("%d", &stairs) == 1) {
		printf("There are %llu ways to go up\n", possibilities(stairs));
	}

	return 0;
}
