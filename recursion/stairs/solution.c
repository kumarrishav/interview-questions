
#include <stdio.h>

unsigned long long possibilities(int left) {
	if (left < 0) {
		return 0;
	}
	if (left == 0) {
		return 1;
	}

	return possibilities(left-1)+possibilities(left-2)+possibilities(left-3);
}

int main(void) {
	printf("Enter number of stairs\n");

	int stairs;
	while (scanf("%d", &stairs) == 1) {
		printf("There are %llu ways to go up\n", possibilities(stairs));
	}

	return 0;
}
