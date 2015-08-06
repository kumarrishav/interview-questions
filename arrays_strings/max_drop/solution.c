
#include <stdio.h>
#include <assert.h>

#define max(a, b) ((a) > (b) ? (a) : (b))

int max_drop(int arr[], size_t len) {
	assert(len >= 2);
	int max_so_far = arr[0]-arr[1];
	int curr_max = max_so_far;

	size_t i;
	for (i = 1; i < len-1; i++) {
		curr_max = max(arr[i]-arr[i+1], curr_max+arr[i]-arr[i+1]);
		max_so_far = max(max_so_far, curr_max);
	}

	return max_so_far;
}

static int arr[1024];

int main(void) {
	printf("Enter size of array, followed by the elements\n");
	printf("> ");

	size_t arr_len;
	while (scanf("%zu", &arr_len) == 1) {
		size_t i;
		for (i = 0; i < arr_len; i++)
			scanf("%d", &arr[i]);
		if (arr_len < 2)
			printf("Arrays must have at least 2 elements.\n");
		else
			printf("Max drop = %d\n", max_drop(arr, arr_len));
		printf("> ");
	}

	return 0;
}
