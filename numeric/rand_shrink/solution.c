
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* Returns a random integer in the range [0..max]
 * with the same distribution as stdlib's rand()
 *
 * `max` must be less than or equal to `RAND_MAX`
 */
int rand_zero(int max) {
	if (max > RAND_MAX) {
		return -1;
	} else if (max == RAND_MAX) {
		return rand();
	}

	int excess = (RAND_MAX%(max+1)+1)%(max+1);
	int lim = RAND_MAX-excess;

	int ret;
	for (ret = rand(); ret > lim; ret = rand())
		; /* Intentionally left blank */

	return ret%(max+1);
}

/* Returns a random integer in the range [low..high]
 * with the same distribution as stdlib's rand()
 */
int range_rand(int low, int high) {
	return rand_zero(high-low)+low;
}

int main(int argc, char *argv[]) {
	unsigned seed;

	if (argc > 1) {
		if (sscanf(argv[1], "%u", &seed) != 1) {
			fprintf(stderr, "Invalid seed \"%s\". Please use an integer >= 0.\n", argv[1]);
			exit(EXIT_FAILURE);
		}
	} else {
		seed = (unsigned) time(NULL);
	}

	srand(seed);

	printf("Enter a range [l, r], where l >= 0 and r <= %d, followed by the number of iterations\n", RAND_MAX);
	printf("> ");

	int l, h, i;
	while (scanf("%d%d%d", &l, &h, &i) == 3) {
		while (i--) {
			printf("%d\n", range_rand(l, h));
		}
		printf("> ");
	}

	return 0;
}
