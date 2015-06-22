
#include <stdio.h>
#include <assert.h>

static size_t find_max(size_t arr_sz, int arr[arr_sz]) {

	if (arr_sz <= 1) {
		return 0;
	}

	ssize_t l = 0;
	ssize_t r = arr_sz-1;
	ssize_t m = l+(r-l)/2;

	while (!(m == 0 || (arr[m-1] < arr[m] && arr[m+1] < arr[m]))) {
		if (arr[m-1] > arr[m]) {
			r = m-1;
		} else {
			l = m+1;
		}
		m = l+(r-l)/2;
	}

	if (m == 0) {
		return 1;
	} else {
		return m;
	}
}

ssize_t find_elem(size_t arr_sz, int arr[arr_sz], int val) {
	size_t max = find_max(arr_sz, arr);

	ssize_t l;
	ssize_t r;

	// Search on left
	l = 0;
	r = max;

	while (l <= r) {
		ssize_t m = l+(r-l)/2;
		if (arr[m] == val) {
			return m;
		} else if (arr[m] < val) {
			l = m+1;
		} else {
			r = m-1;
		}
	}

	// Search on right
	l = max;
	r = arr_sz-1;

	while (l <= r) {
		ssize_t m = l+(r-l)/2;
		if (arr[m] == val) {
			return m;
		} else if (arr[m] < val) {
			r = m-1;
		} else {
			l = m+1;
		}
	}

	return -1;
}

static int array_buf[1024];

int main(void) {
	printf("Enter array size, followed by the elements, followed by element to find\n");
	printf("> ");

	size_t arr_sz;
	while (scanf("%zu", &arr_sz) == 1) {
		size_t i;
		for (i = 0; i < arr_sz; i++) {
			scanf("%d", &array_buf[i]);
		}

		int val;
		scanf("%d", &val);
		ssize_t res = find_elem(arr_sz, array_buf, val);

		if (res == -1) {
			printf("Not found.\n");
		} else {
			printf("Found at array[%zd]\n", res);
		}

		printf("> ");
	}

	return 0;
}
