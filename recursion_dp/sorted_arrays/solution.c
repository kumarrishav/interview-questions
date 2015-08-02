
#include <stdio.h>

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
