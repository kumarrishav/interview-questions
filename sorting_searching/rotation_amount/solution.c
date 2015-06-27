/* A non decreasing sorted array is rotated by some random amount.
 *
 * Write a function to find out the amount on which the array was rotated.
 * Assume clockwise rotations only.
 *
 * EXAMPLE
 *
 * Input: [ 6, 7, 8, 1, 2, 3, 4, 5 ]
 * Output: 3
 *
 * Input: [ 2, 2, 1, 1, 1, 2 ]
 * Output: 2
 *
 * Source: Careercup (Microsoft interview)
 */
#include <stdio.h>

ssize_t find_rotation_amount(int arr[], ssize_t l, ssize_t r, ssize_t len) {

	ssize_t mid = l+(r-l)/2;

	if (mid >= 0 && mid < len-1 && arr[mid] > arr[mid+1]) {
		return mid+1;
	}

	if (l >= r) {
		return -1;
	}

	if (arr[l] > arr[r] ) {
		if (arr[mid] < arr[l]) {
			return find_rotation_amount(arr, l, mid-1, len);
		} else {
			return find_rotation_amount(arr, mid+1, r, len);
		}
	}

	ssize_t res;
	res = find_rotation_amount(arr, l, mid-1, len);
	if (res == -1) {
		res = find_rotation_amount(arr, mid+1, r, len);
	}

	return res;
}

static int array_buff[1024];

int main(void) {
	printf("Enter size of array, followed by the elements\n");
	printf("> ");

	size_t len;
	while (scanf("%zu", &len) == 1) {
		size_t i;
		for (i = 0; i < len; i++) {
			scanf("%d", &array_buff[i]);
		}

		printf("Rotation amount = %zd\n", find_rotation_amount(array_buff, 0, len-1, len));
		printf("> ");

	}

	return 0;
}
