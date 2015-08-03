/* There are two sorted arrays, A and B, of the same size.
 *
 * Generate all possible sorted arrays that result from alternating between picking an element from
 * A and B; and all sorted arrays that result from alternating between B and A.
 *
 * EXAMPLE
 *
 * Input:
 *
 * A = [ 0, 1, 5 ]
 * B = [ 2, 7, 10 ]
 *
 * Output:
 *
 * [ 0, 2, 5, 7 ]
 * [ 0, 2, 5, 10 ]
 * [ 0, 7 ]
 * [ 0, 10 ]
 * [ 2, 5, 7 ]
 * [ 2, 5, 10 ]
 *
 * Explanation:
 *
 * The first 4 arrays in the result set are all the possibilities if the first element we pick is
 * from array A.
 * The last 2 arrays in the result set are all the possibilities if the first element we pick is
 * from array B.
 *
 * Source: Careercup (Microsoft interview)
 */
#include <stdio.h>

/* Note:
 * The following solution assumes we don't care about generating duplicate arrays.
 *
 * It also assumes that the question (as shown in the example) always aims to find
 * the longest possible array and only stops when it is not possible to extend it.
 * This can happen because we ran out of elements or because the other array does not
 * have any element that has not been used and is greater than or equal to the last
 * chosen element.
 */

static ssize_t bin_search(int arr[], int val, size_t len) {
	ssize_t l, r;
	l = -1;
	r = len;

	/* Invariant: arr[l] < val <= arr[r] */
	while (l+1 != r) {
		ssize_t m = l+(r-l)/2;
		if (arr[m] < val)
			l = m;
		else
			r = m;
	}

	return r == len ? -1 : r;
}

static void print_array(int arr[], size_t len) {
	if (len > 0)
		printf("[ %d", arr[0]);
	size_t i;
	for (i = 1; i < len; i++)
		printf(", %d", arr[i]);
	printf(" ];\n");
}

static void sorted_arrays_aux(int active[], size_t active_len,
			      int other[], size_t other_len,
			      int out_buff[], size_t out_buff_i) {

	ssize_t other_i;

	if (active_len == 0 || other_len == 0 ||
	    ((other_i = bin_search(other, active[0], other_len)) == -1)) {
		print_array(out_buff, out_buff_i);
		return;
	}

	size_t i;
	for (i = other_i; i < other_len; i++) {
		out_buff[out_buff_i] = other[i];
		sorted_arrays_aux(other+i, other_len-i, active+1, active_len-1, out_buff, out_buff_i+1);
	}
}

void sorted_arrays(int arr_a[], int arr_b[], size_t size) {
	if (size == 0)
		return;

	int out_buff[size*2];

	out_buff[0] = arr_a[0];
	sorted_arrays_aux(arr_a, size, arr_b, size, out_buff, 1);

	out_buff[0] = arr_b[0];
	sorted_arrays_aux(arr_b, size, arr_a, size, out_buff, 1);
}

static int arr_a[1024];
static int arr_b[1024];

int main(void) {
	printf("Enter the size of the arrays (both have the same size), followed by\n");
	printf("A's elements, followed by B's elements\n");
	printf("Note: both arrays must be sorted\n");
	printf("> ");

	size_t arr_sz;
	while (scanf("%zu", &arr_sz) == 1) {
		size_t i;
		for (i = 0; i < arr_sz; i++)
			scanf("%d", &arr_a[i]);
		for (i = 0; i < arr_sz; i++)
			scanf("%d", &arr_b[i]);
		sorted_arrays(arr_a, arr_b, arr_sz);
		printf("> ");
	}

	return 0;
}
