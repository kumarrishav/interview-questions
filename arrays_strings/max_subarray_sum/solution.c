
#include <stdio.h>

int max_subarray_sum(int a[], size_t sz, size_t *begin, size_t *len) {
	int max_so_far = a[0];
	size_t max_so_far_start = 0;
	size_t max_so_far_len = 1;

	int curr_max = a[0];
	size_t curr_max_start = 0;
	size_t curr_max_len = 1;

	size_t i;
	for (i = 1; i < sz; i++) {
		if (a[i] > curr_max+a[i]) {
			curr_max = a[i];
			curr_max_start = i;
			curr_max_len = 1;
		} else {
			curr_max += a[i];
			curr_max_len++;
		}

		if (curr_max > max_so_far) {
			max_so_far = curr_max;
			max_so_far_start = curr_max_start;
			max_so_far_len = curr_max_len;
		}
	}

	*begin = max_so_far_start;
	*len = max_so_far_len;

	return max_so_far;
}

static int array[1024];

int main(void) {
	printf("Enter size of array, followed by the elements.\n");
	printf("> ");

	size_t array_sz;
	while (scanf("%zu", &array_sz) == 1) {
		size_t i;
		for (i = 0; i < array_sz; i++) {
			scanf("%d", &array[i]);
		}

		size_t res_start;
		size_t res_len;
		int res;

		res = max_subarray_sum(array, array_sz, &res_start, &res_len);

		printf("max subarray sum = %d (a[%zd..%zd])\n", res, res_start, res_start+res_len-1);
		printf("> ");
	}

	return 0;
}
