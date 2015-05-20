/* You are given an array of integers, and a target number. You are to write a method that finds and
 * prints all sequences of operations on the array elements that yield the target number.
 * Two arithmetic operations can be used: add (+) and subtract (-).
 *
 * You can't modify the array, and you have to use all elements in the original order.
 *
 * EXAMPLE
 *
 * Input:
 * array  = [ 1, 9, 1, 2 ]
 * target = 9
 *
 * Output:
 * 1+9+1-2
 *
 * Explanation:
 * The only combination that yields 9 is by performing 2 consecutive additions followed by a
 * subtraction.
 *
 * Source: Careercup (Flipkart interview)
 */
#include <stdio.h>
#include <string.h>

unsigned target_n_aux(size_t n, int numbers[n], size_t i, int res_so_far, int target,
		      char *buff, size_t buff_i) {
	if (i == n-1) {
		if (res_so_far == target) {
			printf("%s%d\n", buff, numbers[i]);
			return 1;
		}
		return 0;
	}

	unsigned res = 0;

	buff_i += sprintf(&buff[buff_i], "%d+", numbers[i]);
	res += target_n_aux(n, numbers, i+1, res_so_far+numbers[i+1], target, buff, buff_i);
	buff[buff_i-1] = '-';
	res += target_n_aux(n, numbers, i+1, res_so_far-numbers[i+1], target, buff, buff_i);

	return res;
}

unsigned target_n(size_t n, int numbers[n], int target) {
	if (n == 0) {
		return 0;
	}

	char buff[n*32];
	memset(buff, 0, sizeof(buff));

	return target_n_aux(n, numbers, 0, numbers[0], target, buff, 0);
}

int main(void) {
	printf("Enter the array size, followed by the array elements, followed by the target value.\n");
	printf("> ");

	size_t arr_sz;
	while (scanf("%zu", &arr_sz) == 1) {

		int numbers[arr_sz];

		size_t i;
		for (i = 0; i < arr_sz; i++) {
			scanf("%d", &numbers[i]);
		}

		int target;
		scanf("%d", &target);

		if (target_n(arr_sz, numbers, target) == 0) {
			printf("Impossible.\n");
		}

		printf("> ");
	}

	return 0;
}
