/* A frog only moves forward, but it can move in steps 1 inch long or in jumps 2 inches long. A frog
 * can cover the same distance using different combinations of steps and jumps.
 *
 * Write a function that calculates the number of different combinations a frog can use to cover a
 * given distance.
 *
 * For example, a distance of 3 inches can be covered in three ways: step-step-step, step-jump,
 * and jump-step.
 *
 * Source: StackOverflow
 */

#include <stdio.h>

/* This problem is reduced to the problem of finding the N-th Fibonacci number, which can be solved
 * in O(N) time and O(1) memory
 *
 * See http://stackoverflow.com/questions/32202911
 */
int frog_combos(int n) {
	int before_last = 1, last = 0;
	int i;
	for (i = 0; i < n+1; i++) {
		int new_last = before_last + last;
		before_last = last;
		last = new_last;
	}
	return last;
}

int main(void) {
	printf("Enter a value for N.\n");
	printf("> ");

	int n;
	while (scanf("%d", &n) == 1)
		printf("%d\n> ", frog_combos(n));

	return 0;
}
