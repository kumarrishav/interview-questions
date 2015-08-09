/* Given a number represented in an array, write a function that changes the array to the next
 * smallest number that has the exact same digits. If there is no next smallest number, return 0,
 * otherwise, modify the array accordingly and return 1.
 *
 * EXAMPLE
 *
 * Input:  [ 3, 7, 1 ];
 *
 * Output: [ 3, 1, 7 ];
 *
 * Explanation: The input represents the number 371. The largest number with the digits 3, 7 and 1
 * that is less than 371 is 317. Furthermore, if we repeatedly called next_smallest() until no more
 * numbers exist, we would get the sequence 312, 173, 137.
 *
 * Source: Careercup
 */

#include <stdio.h>

/* The algorithm used here is the reverse of the well known next permutation algorithm
 * (see http://stackoverflow.com/questions/11483060/stdnext-permutation-implementation-explanation)
 *
 * The algorithm for the next permutation of an array A of size N is described as follows:
 *
 * - Find the largest k such that A[k] < A[k+1]. If there is no such k, then there is no next
 *   permutation.
 *
 * - Find the largest l such that A[k] < A[l].
 *
 * - Swap A[k] with A[l]
 *
 * - Reverse A[k+1..N-1]
 *
 * The reverse of this algorithm, which we call next smallest, can be derived from the above.
 * It is the opposite:
 *
 * - Find the largest k such that A[k] > A[k+1]. If there is no such k, then there is no next
 *   smallest permutation.
 *
 * - Find the largest l such that A[k] > A[l].
 *
 * - Swap A[k] with A[l].
 *
 * - Reverse A[k+1..N].
 *
 *
 * Note that while the value of k may be undefined (when there is no next permutation or no next
 * smallest), the value of l is always well defined, because in the former case we're looking for
 * an l such that A[k] < A[l], and A[k] < A[k+1], so there is at least a match. In the latter,
 * we're looking for an l such that A[k] > A[l], and A[k] > A[k+1], so again there is at least
 * one match.
 *
 */

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
