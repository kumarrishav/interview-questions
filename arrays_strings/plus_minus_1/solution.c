
#include <stdio.h>
#define abs(n) ((n) < 0 ? -(n) : (n))

ssize_t find_elem(int arr[], size_t arr_sz, int val) {
	size_t i = 0;

	while (i < arr_sz) {
		if (arr[i] == val) {
			return i;
		}
		i += abs(val-arr[i]);
	}

	return -1;
}

static int array_buf[1024];
int main(void) {
	printf("Enter number of elements in array, followed by array elements, followed by value to find.\n");
	printf("> ");

	size_t arr_sz;
	while (scanf("%zu", &arr_sz) == 1) {
		size_t i;
		for (i = 0; i < arr_sz; i++) {
			scanf("%d", &array_buf[i]);
		}
		int val;
		scanf("%d", &val);
		ssize_t res = find_elem(array_buf, arr_sz, val);
		if (res == -1) {
			printf("Not found.\n");
		} else {
			printf("Found at index %zd\n", res);
		}
		printf("> ");
	}

	return 0;
}
