
#include <stdio.h>
#include <string.h>

/* Without dynamic programming, this would be O(3^N) - terrible! */
unsigned long long possibilities_aux(int left, unsigned long long cached[]) {
	if (left < 0) {
		return 0;
	}
	if (left == 0) {
		return 1;
	}
	if (cached[left-1] != 0) {
		return cached[left-1];
	} else {
		cached[left-1] =
			possibilities_aux(left-1, cached) +
			possibilities_aux(left-2, cached) +
			possibilities_aux(left-3, cached);
	}

	return cached[left-1];
}

unsigned long long possibilities(int stairs) {
	unsigned long long cached[stairs];
	memset(cached, 0, sizeof(cached));
	return possibilities_aux(stairs, cached);
}

int main(void) {
	printf("Enter number of stairs\n");

	int stairs;
	while (scanf("%d", &stairs) == 1) {
		printf("There are %llu ways to go up\n", possibilities(stairs));
	}

	return 0;
}
