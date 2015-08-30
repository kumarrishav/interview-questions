
#include <stdio.h>
#include <assert.h>

void find_two_repeating(int arr[], size_t arr_len, int *a, int *b) {
	assert(arr_len > 3);
	size_t n = arr_len-2;

	int i;
	int xor_n = 0;
	for (i = 1; i <= n; i++)
		xor_n ^= i;

	int xor_dups = 0;
	for (i = 0; i < arr_len; i++)
		xor_dups ^= arr[i];
	xor_dups ^= xor_n;

	int diff_bit = xor_dups & -xor_dups;
	*a = 0;
	*b = 0;

	for (i = 0; i < arr_len; i++)
		if (arr[i] & diff_bit)
			*a ^= arr[i];
		else
			*b ^= arr[i];

	for (i = 1; i <= n; i++)
		if (i & diff_bit)
			*a ^= i;
		else
			*b ^= i;
}

int main(void) {
	static int arr[1024];
	size_t arr_len;

	printf("Enter the size of the array, followed by each element.\n");
	printf("> ");

	while (scanf("%zu", &arr_len) == 1) {
		size_t i;
		for (i = 0; i < arr_len; i++)
			scanf("%d", &arr[i]);
		int a, b;
		find_two_repeating(arr, arr_len, &a, &b);
		printf("%d %d\n> ", a, b);
	}

	return 0;
}
