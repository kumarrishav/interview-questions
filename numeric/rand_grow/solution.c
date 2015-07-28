
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
