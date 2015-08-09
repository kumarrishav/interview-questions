
#include <stdio.h>

static void swap(int arr[], size_t i, size_t j) {
	int tmp = arr[i];
	arr[i] = arr[j];
	arr[j] = tmp;
}

static void reverse(int arr[], size_t len) {

	if (len == 0)
		return;

	size_t i, j;
	for (i = 0, j = len-1; i < j; i++, j--)
		swap(arr, i, j);
}

int next_smallest(int arr[], size_t len) {
	if (len <= 1)
		return 0;

	ssize_t k = -1;
	ssize_t l = -1;

	ssize_t i;
	for (i = len-2; i >= 0 && arr[i] <= arr[i+1]; i--)
		; /* Intentionally left blank */

	k = i;

	if (k == -1)
		return 0;

	for (i = len-1; arr[k] <= arr[i]; i--)
		; /* Intentionally left blank */

	l = i;

	swap(arr, k, l);
	reverse(&arr[k+1], len-k-1);

	return 1;
}

static void print_array(int arr[], size_t len) {
	if (len > 0)
		printf("[ %d", arr[0]);
	size_t i;
	for (i = 1; i < len; i++)
		printf(", %d", arr[i]);
	printf(" ];\n");
}

static int arr_buff[1024];

int main(void) {
	printf("Enter size of array, followed by the elements\n");
	printf("> ");

	size_t arr_sz;
	while (scanf("%zu", &arr_sz) == 1) {
		size_t i;
		for (i = 0; i < arr_sz; i++)
			scanf("%d", &arr_buff[i]);

		print_array(arr_buff, arr_sz);
		while (next_smallest(arr_buff, arr_sz))
			print_array(arr_buff, arr_sz);

		printf("> ");
	}

	return 0;
}
