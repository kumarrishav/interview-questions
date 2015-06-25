/* Given an array where every element occurs exactly three times, except for one of them,
 * write a function to find that element.
 *
 * Can you do it in O(N) time and O(1) memory?
 *
 * Source: InterviewBit (Google Interview)
 */
#include <stdio.h>
#include <assert.h>

int find_number(int arr[], size_t n) {
	size_t i;
	int ones, twos, mask;

	ones = twos = 0;
	for (i = 0; i < n; i++) {
		mask = arr[i] & ~twos;
		ones ^= mask;
		twos ^= arr[i] & ~mask;
		mask = (mask & ones) ^ mask;
		twos ^= mask;
		assert((ones & twos) == 0);
	}

	assert(ones == 0 || twos == 0);

	return twos == 0 ? ones : twos;
}

static int arr_buff[1024];

int main(void) {
	printf("Enter size of array, followed by elements. Every element must be repeated 3 times except for one of them.\n");
	printf("> ");

	size_t n;
	while (scanf("%zu", &n) == 1) {
		size_t i;
		for (i = 0; i < n; i++) {
			scanf("%d", &arr_buff[i]);
		}

		printf("%d\n", find_number(arr_buff, n));
		printf("> ");
	}

	return 0;
}
