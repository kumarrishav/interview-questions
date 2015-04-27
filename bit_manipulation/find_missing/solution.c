/* An array A contians all the integers from 0 to n, except for one number which is
 * missing. In this problem, we cannot access an entire integer in A with a single
 * operation. The elements of A are represented in binary, and the only operation
 * we can use to access them is "fetch the jth bit of A[i]", which takes constant
 * time. Write code to find the missing integer. Can you do it in O(n) time?
 *
 * Source: Cracking the Coding Interview, page 92, question 5.7
 */

#include <stdio.h>
#include <limits.h>

unsigned find_missing(unsigned char bit_array[], size_t array_sz, size_t col) {
	if (col == CHAR_BIT) {
		return 0;
	}

	unsigned char zeros[array_sz/2+1];
	size_t zeros_i = 0;

	unsigned char ones[array_sz/2+1];
	size_t ones_i = 0;

	size_t i;
	for (i = 0; i < array_sz; i++) {
		if (bit_array[i] & (1 << col)) {
			ones[ones_i++] = bit_array[i];
		} else {
			zeros[zeros_i++] = bit_array[i];
		}
	}

	if (zeros_i <= ones_i) {
		unsigned v = find_missing(zeros, zeros_i, col+1);
		return v << 1;
	} else {
		unsigned v = find_missing(ones, ones_i, col+1);
		return (v << 1) | 1;
	}
}

static unsigned char arr[1024];
static size_t arr_sz;

int main(void) {
	printf("Enter the size of the array, and then enter each array element.\n"
	       "For an array of size n, there is exactly one missing number of the\n"
	       "sequence 0..n. All the other numbers must be in the array.\n");

	while (scanf("%zu", &arr_sz) == 1) {
		size_t i;
		for (i = 0; i < arr_sz; i++) {
			unsigned x;
			scanf("%u", &x);
			arr[i] = x;
		}

		printf("Missing value is: %u\n", find_missing(arr, arr_sz, 0));
	}

	return 0;
}
