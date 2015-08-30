/* Given an array of n+2 integers that contains every integer in the range [1 .. n] such that
 * exactly two integers are repeated, write a function to find the two elements that appear
 * twice.
 *
 * EXAMPLE:
 *
 * Input array: [4, 2, 4, 5, 2, 3, 1] (7 elements, n = 5, range = [1 .. 5])
 *
 * Output: 2, 4
 *
 * Explanation: n = 5, so the array contains every element in the range [1 .. 5]. The repeated
 *              numbers are 2 and 4.
 *
 * Source: StackOverflow
 */

#include <stdio.h>
#include <assert.h>

/* We use XORs to solve this problem in O(N) time using O(1) memory.
 *
 * The algorithm works as follows:
 *
 * Since we know that the array contains every element in the range [1 .. n],
 * we start by XORing every element in the array together and then XOR the result
 * with every element in the range [1 .. n]. Because of the XOR properties, the unique
 * elements cancel out and the result is the XOR of the duplicated elements (because
 * the duplicate elements have been XORed 3 times in total, whereas all the others were
 * XORed twice and canceled out). This is stored in xor_dups.
 *
 * Next, find a bit in xor_dups that is a 1. Again, due to XOR's properties, a bit set to 1
 * in xor_dups means that that bit is different in the binary representation of the duplicate
 * numbers. Any bit that is a 1 can be picked, we chose to pick the least significant. This
 * is stored in diff_bit.
 *
 * Now, split the array elements into two groups: one group contains the numbers that have a 0
 * bit on the position of the 1-bit that we picked from xor_dups. The other group contains the
 * numbers that have a 1-bit instead. Since this bit is different in the numbers we're looking for,
 * they can't both be in the same group. Furthermore, both occurrences of each number go to the same
 * group.
 *
 * So now we're almost done. Consider the group for the elements with the 0-bit. XOR them all
 * together, then XOR the result with all the elements in the range [1..n] that have a 0-bit on that
 * position, and the result is the duplicate number of that group (because there's only one number
 * repeated inside each group, all the non-repeated numbers canceled out because each one was XORed
 * twice except for the repeated number which was XORed three times).
 *
 * Rinse, repeat: for the group with the 1-bit, XOR them all together, then XOR the result with all
 * the elements in the range [1..n] that have a 1-bit on that position, and the result is the other
 * duplicate number.
 *
 */

void find_two_repeating(int arr[], size_t arr_len, int *a, int *b) {
	assert(arr_len > 3);
	size_t n = arr_len-2;
	int i;

	int xor_dups = 0;
	for (i = 0; i < arr_len; i++)
		xor_dups ^= arr[i];
	for (i = 1; i <= n; i++)
		xor_dups ^= i;

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
