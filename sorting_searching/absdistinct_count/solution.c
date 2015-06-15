
#include <stdio.h>
#include <assert.h>

static unsigned abs_distinct_merge(int arr[], size_t arr_sz, int idx_positive, int idx_negative, int last) {
	unsigned res = 0;
	while (idx_negative >= 0 && idx_positive < arr_sz) {
		int next;
		if (-arr[idx_negative] < arr[idx_positive]) {
			next = -arr[idx_negative];
			idx_negative--;
		} else {
			next = arr[idx_positive];
			idx_positive++;
		}
		if (next != last) {
			res++;
		}
		last = next;
	}

	while (idx_negative >= 0) {
		if (last != -arr[idx_negative]) {
			res++;
		}
		last = -arr[idx_negative];
		idx_negative--;
	}

	while (idx_positive < arr_sz) {
		if (last != arr[idx_positive]) {
			res++;
		}
		last = arr[idx_positive];
		idx_positive++;
	}

	return res;
}

unsigned abs_distinct(int arr[], int arr_sz) {
	int right_neg;
	for (right_neg = arr_sz-1; right_neg >= 0 && arr[right_neg] > 0; right_neg--)
		; /* Intentionally left blank */

	int idx_positive = right_neg+1;
	int idx_negative = right_neg;
	int last;

	if (idx_negative >= 0 && idx_positive < arr_sz) {
		if (-arr[idx_negative] < arr[idx_positive]) {
			last = -arr[idx_negative];
			idx_negative--;
		} else {
			last = arr[idx_positive];
			idx_positive++;
		}
	} else if (idx_negative >= 0) {
		last = -arr[idx_negative];
		idx_negative--;
	} else if (idx_positive < arr_sz) { 
		last = arr[idx_positive];
		idx_positive++;
	} else {
		assert(0);
	}

	return abs_distinct_merge(arr, arr_sz, idx_positive, idx_negative, last)+1;
}

static int array_buf[1024];
int main(void) {
	printf("Enter array size, followed by the elements. Array must be sorted.\n");
	printf("> ");

	size_t array_sz;
	while (scanf("%zu", &array_sz) == 1) {
		assert(array_sz <= sizeof(array_buf)/sizeof(array_buf[0]));
		size_t i;
		for (i = 0; i < array_sz; i++) {
			scanf("%d", &array_buf[i]);
		}
		unsigned distcount = abs_distinct(array_buf, array_sz);
		printf("Absolute distinct count = %u\n", distcount);
		printf("> ");
	}

	return 0;
}
