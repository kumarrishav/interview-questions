/* Write a function that generates a sorted sequence of unique random integers in an interval [0..N]
 *
 * Source: Adapted from Programming Pearls
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

/* Note about rand_zero() and range_rand():
 * See numeric/rand_shrink to learn how it works
 */

/* Returns a random integer in the range [0..max]
 * with the same distribution as stdlib's rand()
 *
 * `max` must be less than or equal to `RAND_MAX`
 */
static int rand_zero(int max) {
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

/* Returns a random integer in the range [low..high]
 * with the same distribution as stdlib's rand()
 */
static int range_rand(int low, int high) {
	if (high-low+1 > RAND_MAX) {
		return -1;
	}
	return rand_zero(high-low)+low;
}

static void swap(int arr[], size_t i, size_t j) {
	int tmp = arr[i];
	arr[i] = arr[j];
	arr[j] = tmp;
}

static void quicksort(int arr[], size_t arr_sz) {
	if (arr_sz <= 1) {
		return;
	}

	size_t mid = arr_sz/2;

	swap(arr, 0, mid);
	size_t i = 0;
	size_t j;
	for (j = 1; j < arr_sz; j++) {
		if (arr[j] <= arr[0]) {
			i++;
			swap(arr, i, j);
		}
	}

	swap(arr, 0, i);

	quicksort(arr, i);
	if (i > 0 && i-1 < arr_sz) {
		quicksort(&arr[i+1], arr_sz-i-1);
	}
}

void sorted_rand_subset(int arr[], size_t arr_sz, size_t max) {
	int *vals = malloc(sizeof(*vals)*(max+1));
	assert(vals != NULL);

	size_t i;
	for (i = 0; i < max+1; i++) {
		vals[i] = i;
	}

	for (i = 0; i < arr_sz; i++) {
		swap(vals, i, range_rand(i, max));
	}

	memcpy(arr, vals, sizeof(arr[0])*arr_sz);
	quicksort(arr, arr_sz);

	free(vals);
}

/* An alternative approach: Knuth's Algorithm S */
void knuth_rand_subset(int arr[], size_t arr_sz, size_t max) {
	size_t missing;
	size_t remaining;

	missing = arr_sz;
	remaining = max+1;

	size_t i = 0;
	size_t j = 0;

	while (missing != 0) {
		if (range_rand(0, remaining-1) < missing) {
			arr[j++] = i;
			missing--;
		}
		remaining--;
		i++;
	}

}

static void print_array(int arr[], size_t sz) {
	printf("[ ");
	if (sz > 0) {
		printf("%d", arr[0]);
	}
	size_t i;
	for (i = 1; i < sz; i++) {
		printf(", %d", arr[i]);
	}
	printf(" ]\n");
}

int main(void) {
	srand(time(NULL));

	printf("Enter a number M and a number N to generate a sorted sequence of M unique random values in the range [0..N]\n");
	printf("Note: N >= M\n");
	printf("> ");

	size_t m, n;
	while (scanf("%zu%zu", &m, &n) == 2) {
		int array[m];
		sorted_rand_subset(array, m, n);
		printf("Suffle method: ");
		print_array(array, m);

		printf("Knuth's Algorithm S: ");
		knuth_rand_subset(array, m, n);
		print_array(array, m);
		printf("> ");
	}

	return 0;
}
