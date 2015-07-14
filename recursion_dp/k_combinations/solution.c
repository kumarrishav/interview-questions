/* Given an array of integers and a number K, write a function to print every combination of exactly
 * K elements of that array.
 *
 * EXAMPLE
 * Input:  { 1, 2, 3 }
 *         K = 2
 *
 * Output:
 * { 1, 2 }
 * { 1, 3 }
 * { 2, 3 }
 *
 * Any order is valid, as long as every combination is generated
 *
 * Source: Stackoverflow
 */

#include <stdio.h>
#include <assert.h>

static char out_buff[1024];
static size_t buff_i;

void k_combinations_aux(int arr[], size_t arr_len, size_t curr_i, size_t k) {
	if (k == 0) {
		out_buff[buff_i] = '\0';
		printf("{ %s}\n", out_buff);
		return;
	}

	if (curr_i == arr_len) {
		return;
	}

	if (arr_len-curr_i > k) {
		k_combinations_aux(arr, arr_len, curr_i+1, k);
	}

	int added = sprintf(&out_buff[buff_i], "%d ", arr[curr_i]);
	buff_i += added;
	k_combinations_aux(arr, arr_len, curr_i+1, k-1);
	buff_i -= added;
}

void k_combinations(int arr[], size_t arr_len, size_t k) {
	buff_i = 0;
	k_combinations_aux(arr, arr_len, 0, k);
}

static int arr[1024];

int main(void) {
	printf("Enter array size, followed by the elements, followed by K\n");
	printf("> ");

	size_t arr_sz;
	while (scanf("%zu", &arr_sz) == 1) {
		assert(arr_sz < sizeof(arr)/sizeof(arr[0]));
		size_t i;
		for (i = 0; i < arr_sz; i++) {
			scanf("%d", &arr[i]);
		}
		size_t k;
		scanf("%zu", &k);
		k_combinations(arr, arr_sz, k);
		printf("> ");
	}

	return 0;
}
