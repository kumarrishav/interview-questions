/* You are given a function rand5() that uniformly generates a random number in the interval [1..5]
 *
 * Write a function rand7() that uses rand5() to generate a uniform random number in the range
 * [1..7]
 *
 * Source: StackOverflow
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* A good way to visualize the solution to this problem is to think of the values in [1..7] as the
 * numbers in a dart board target.
 *
 * Since we want to grow the range of rand5() uniformly, we will have to call rand5() at least
 * twice. Doing so enables us to get a uniform random number in the range [0..24] as follows:
 *
 * n = 5*(rand5()-1)+(rand5()-1)
 *
 * Essentially, we can think of it as selecting a random cell position in this matrix:
 *
 * [ 1, 2, 3, 4, 5 ]
 * [ 6, 7, 1, 2, 3 ]
 * [ 4, 5, 6, 7, 1 ]
 * [ 2, 3, 4, 5, 6 ]
 * [ 7, 1, 2, 3, 4 ]
 *
 * Note that the formula above gives the flattened offset of a random position in the matrix
 * (it's as if we picked row i and column j, and computed the absolute offset with n = 5*i+j).
 *
 * However, the numbers 1, 2, 3 and 4 show up one more time than all of the others, so we can't
 * naively return n%7. Instead, we keep shooting darts towards the target, ignoring any cell
 * position greater than 20 (the last 4 positions).
 *
 * This ensures a uniform distribution in the range [0..20], which has 21 elements and thus is
 * evenly divisible by 7, so we can take the value modulo 7 at that point.
 *
 */

int rand5(void);

int rand7(void) {
	int n = 5*(rand5()-1)+(rand5()-1);
	while (n > 20)
		n = 5*(rand5()-1)+(rand5()-1);
	return (n%7)+1;
}

int main(void) {
	srand(time(NULL));

	printf("Enter how many numbers you want to generate\n");
	printf("> ");

	unsigned n;
	while (scanf("%u", &n) == 1) {
		int i;
		for (i = 0; i < n; i++)
			printf("%d\n", rand7());
		printf("> ");
	}

	return 0;
}

/* ~~~~~~~~~~~~~~~~~~~~~~
 * rand5() implementation
 * ~~~~~~~~~~~~~~~~~~~~~~
 */

int rand_zero(int max);

/* Returns a random integer in the range [low..high]
 * with the same distribution as stdlib's rand()
 */
int range_rand(int low, int high) {
	if (high-low+1 > RAND_MAX) {
		return -1;
	}
	return rand_zero(high-low)+low;
}

int rand_zero(int max) {
	if (max > RAND_MAX) {
		return -1;
	} else if (max == RAND_MAX) {
		return rand();
	}

	int excess = RAND_MAX%(max+1)+1;
	int lim = RAND_MAX-excess;

	int ret;
	for (ret = rand(); ret > lim; ret = rand())
		; /* Intentionally left blank */

	return ret%(max+1);
}

int rand5(void) {
	return range_rand(1, 5);
}
