/* Given a source array and a target array with the same elements, determine the minimum number
 * of swaps to transform source into target.
 *
 * Source: invented
 */
#include <stdio.h>
#include <assert.h>

static void swap(int arr[], size_t i, size_t j) {
	int tmp = arr[i];
	arr[i] = arr[j];
	arr[j] = tmp;
}

static int min_swaps_aux(int from[], int to[], size_t sz, size_t curr, int swaps) {
	if (curr == sz) {
		size_t i;
		for (i = 0; i < sz && from[i] == to[i]; i++)
			; /* Intentionally left blank */
		return i < sz ? -1 : swaps;
	}

	int res = min_swaps_aux(from, to, sz, curr+1, swaps);

	size_t i;
	for (i = curr+1; i < sz; i++) {
		swap(from, curr, i);
		int s = min_swaps_aux(from, to, sz, curr+1, swaps+1);
		if (res == -1 || (s != -1 && s < res)) {
			res = s;
		}
		swap(from, curr, i);
	}

	return res;

}

int min_swaps(int from[], int to[], size_t sz) {
	return min_swaps_aux(from, to, sz, 0, 0);
}

static int array[1024];
static int array_target[1024];

int main(void) {
	printf("Enter number of elements in array, followed by the source array, followed by the target array.\n");
	printf("> ");

	size_t elems;
	while (scanf("%zu", &elems) == 1) {
		size_t i;
		for (i = 0; i < elems; i++) {
			scanf("%d", &array[i]);
		}
		for (i = 0; i < elems; i++) {
			scanf("%d", &array_target[i]);
		}

		int res = min_swaps(array, array_target, elems);
		assert(res != -1);

		printf("Swaps: %d\n", res);
		printf("> ");

	}

	return 0;
}
