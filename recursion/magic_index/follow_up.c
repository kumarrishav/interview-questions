/* FOLLOW UP
 * What if the values are not distinct?
 *
 * Source: Cracking the Coding Interview, page 109, question 9.3
 */
#include <stdio.h>
#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

ssize_t magic_index_aux(int arr[], ssize_t arr_sz, ssize_t l, ssize_t r) {

	if (l > r || l < 0 || r > arr_sz-1) {
		return -1;
	}

	ssize_t m = l+(r-l)/2;

	if (arr[m] == m) {
		return m;
	}

	ssize_t ret = magic_index_aux(arr, arr_sz, l, min(m-1, arr[m]));
	if (ret == -1) {
		ret = magic_index_aux(arr, arr_sz, max(m+1, arr[m]), r);
	}

	return ret;
}

ssize_t magic_index(int arr[], size_t arr_sz) {
	return magic_index_aux(arr, arr_sz, 0, arr_sz-1);
}

static int array[256];
size_t array_sz;

int main(void) {
	printf("Enter array size, followed by array elements\n");

	while (scanf("%zu", &array_sz) == 1) {
		size_t i;
		for (i = 0; i < array_sz; i++) {
			scanf("%d", &array[i]);
		}

		ssize_t magic_i = magic_index(array, array_sz);

		if (magic_i == -1) {
			printf("No magic index\n");
		} else {
			printf("Magic index = %zd\n", magic_i);
		}
	}

	return 0;
}

