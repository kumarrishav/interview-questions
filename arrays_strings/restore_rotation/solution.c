/* You are given an array that has been rotated an unknown number of times.
 *
 * Originally, the array was sorted in increasing order.
 * Write a function to find the rotation spot. The rotation spot is the index i
 * such that array[i] > array[i+1].
 *
 * If no such index exists, the function should return -1.
 *
 * FOLLOW UP
 * How would you restore the array back to its original configuration?
 *
 * Source: Careercup (Google interview)
 */
#include <stdio.h>
#include <assert.h>

static int find_rotation_spot_aux(int arr[], int l, int r) {

	while (l+1 < r) {
		int m = l+(r-l)/2;

		if (arr[l] < arr[m]) {
			l = m;
		} else if (arr[m] < arr[r]) {
			r = m;
		} else {
			int res_l = find_rotation_spot_aux(arr, l, m);
			if (res_l != -1) {
				return res_l;
			} else {
				return find_rotation_spot_aux(arr, m, r);
			}
		}
	}

	if (l+1 > r || arr[l] <= arr[l+1]) {
		return -1;
	} else {
		return l;
	}
}

int find_rotation_spot(int arr[], size_t n) {
	if (n == 0) {
		return -1;
	}

	return find_rotation_spot_aux(arr, 0, n-1);
}

static void reverse(int arr[], size_t begin, size_t end) {
	while (begin < end) {
		end--;
		int tmp = arr[begin];
		arr[begin] = arr[end];
		arr[end] = tmp;
		begin++;
	}
}

void restore_array(int arr[], size_t n, int rot_spot) {
	if (rot_spot != -1) {
		reverse(arr, 0, rot_spot+1);
		reverse(arr, rot_spot+1, n);
		reverse(arr, 0, n);
	}
}

static void print_array(int arr[], size_t n) {
	printf("[ ");
	if (n > 0) {
		printf("%d", arr[0]);
	}
	size_t i;
	for (i = 1; i < n; i++) {
		printf(", %d", arr[i]);
	}
	printf(" ];");
}

int main(void) {
	printf("Enter array size, followed by the array elements\n");
	printf("> ");

	size_t arr_sz;
	while (scanf("%zu", &arr_sz) == 1) {
		int array[arr_sz];
		size_t i;
		for (i = 0; i < arr_sz; i++) {
			scanf("%d", &array[i]);
		}

		int pos = find_rotation_spot(array, arr_sz);

		printf("Rotation spot = %d\n", pos);
		restore_array(array, arr_sz, pos);
		printf("Restored array: ");
		print_array(array, arr_sz);
		printf("\n> ");
	}

	return 0;
}
