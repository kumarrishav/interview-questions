/* Based off the problem in Column 1 of Programming Pearls:
 * Sort 10-million integers of 7 digits each using about 1 MB of memory
 * No data is associated with each integer, and there are no duplicates
 */

#include <stdio.h>
#include <limits.h>
#include <assert.h>
#define MAX_INPUT_SZ 10000000

static char bit_vector[(MAX_INPUT_SZ-1)/CHAR_BIT+1];

#define set_bit(a, x) ((a)[(x)/CHAR_BIT] |= 1 << ((x)%CHAR_BIT))
#define is_set(a, x) ((a)[(x)/CHAR_BIT] & 1 << ((x)%CHAR_BIT))
void clear_bit_vector(char bv[], size_t sz) {
	size_t i;
	for (i = 0; i < sz; i++) {
		bv[i] = 0;
	}
}

/* Sorts a set of unique numbers in the range [0, MAX_INPUT_SZ) */
void countingsort(unsigned arr[], size_t arr_sz) {
	clear_bit_vector(bit_vector, sizeof bit_vector);

	size_t i;
	for (i = 0; i < arr_sz; i++) {
		assert(0 <= arr[i] && arr[i] < MAX_INPUT_SZ);
		set_bit(bit_vector, arr[i]);
	}

	size_t j = 0;
	for (i = 0; i < MAX_INPUT_SZ; i++) {
		if (is_set(bit_vector, i)) {
			arr[j++] = i;
		}
	}
}

#define MAX_ARR_SZ 256
static unsigned array[MAX_ARR_SZ];
static size_t array_sz;

static void print_array(unsigned arr[], size_t arr_sz) {
	size_t i;
	for (i = 0; i < arr_sz; i++) {
		printf("%u ", arr[i]);
	}
	printf("\n");
}

int main(void) {
	printf("Input format: N n1 n2 n3 ...\n");
	printf("N is the array size; the rest are the elements\n");

	while (scanf("%zu", &array_sz) == 1) {
		size_t i;
		for (i = 0; i < array_sz; i++) {
			scanf("%u", &array[i]);
		}
		printf("Received: ");
		print_array(array, array_sz);
		printf("Sorted: ");
		countingsort(array, array_sz);
		print_array(array, array_sz);
	}

	return 0;
}

