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
 * Source: Careercup (flipkart interview)
 */
#include <stdio.h>
#include <assert.h>

static int eval(size_t n, int numbers[n], char ops[n-1]) {
	int res = numbers[0];

	size_t i;
	for (i = 0; i < n-1; i++) {
		if (ops[i] == '+') {
			res += numbers[i+1];
		} else if (ops[i] == '-') {
			res -= numbers[i+1];
		} else {
			assert(0);
		}
	}

	return res;
}

static int next_ops(size_t n, char ops[n]) {
	int i = n-1;

	while (i >= 0 && ops[i] == '-') {
		ops[i] = '+';
		i--;
	}

	if (i < 0) {
		return 0;
	} else {
		ops[i] = '-';
		return 1;
	}
}

static void print_solution(size_t n, int numbers[n], char ops[n-1]) {
	printf("%d", numbers[0]);

	size_t i;
	for (i = 0; i < n-1; i++) {
		printf("%c%d", ops[i], numbers[i+1]);
	}

	printf("\n");
}

unsigned target_n(size_t n, int numbers[n], int target) {
	if (n == 0) {
		return 0;
	}

	unsigned res = 0;

	char ops[n-1];
	size_t op_i;
	for (op_i = 0; op_i < n-1; op_i++) {
		ops[op_i] = '+';
	}

	if (eval(n, numbers, ops) == target) {
		print_solution(n, numbers, ops);
		res++;
	}

	while (next_ops(n-1, ops)) {
		if (eval(n, numbers, ops) == target) {
			print_solution(n, numbers, ops);
			res++;
		}
	}

	return res;
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
