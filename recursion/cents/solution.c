
#include <stdio.h>

unsigned ways(int n) {
	if (n < 0) {
		return 0;
	}
	if (n == 0) {
		return 1;
	}

	return ways(n-25)+ways(n-10)+ways(n-5)+ways(n-1);
}

int main(void) {
	int n;
	printf("Enter the cents quantity\n");
	printf("> ");
	fflush(stdout);

	while (scanf("%d", &n) == 1) {
		unsigned res = ways(n);
		if (res == 1) {
			printf("There's 1 way to represent %d.\n", n);
		} else if (res == 0) {
			printf("Can't represent %d\n", res);
		} else {
			printf("There are %u ways to represent %d.\n", res, n);
		}

		printf("> ");
		fflush(stdout);
	}

	return 0;
}
