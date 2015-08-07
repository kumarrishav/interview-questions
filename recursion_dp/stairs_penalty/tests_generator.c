/* Tests generator for "Stairs Penalty" */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_N 5000
#define MIN_N 500
#define MAX_COST 10

int main(void) {
	srand(time(NULL));

	int n = (rand()%(MAX_N-MIN_N+1))+MIN_N;

	printf("%d\n", n);

	int i;

	printf("%d", (rand()%MAX_COST)+1);
	for (i = 1; i < n; i++)
		printf(" %d", (rand()%MAX_COST)+1);
	printf("\n");

	printf("%d", (rand()%MAX_COST)+1);
	for (i = 1; i < n-1; i++)
		printf(" %d", (rand()%MAX_COST)+1);
	printf("\n");

	printf("%d", (rand()%MAX_COST)+1);
	for (i = 1; i < n-1; i++)
		printf(" %d", (rand()%MAX_COST)+1);
	printf("\n\n");

	return 0;
}
