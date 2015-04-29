
#include <stdio.h>

void hanoi(int source, int target, int auxiliary, int disks) {
	if (disks == 0) {
		return;
	}

	hanoi(source, auxiliary, target, disks-1);
	printf("Move from tower %d to %d\n", source, target);
	hanoi(auxiliary, target, source, disks-1);
}

int main(void) {

	printf("How many disks?\n");
	int disks;
	while (scanf("%d", &disks) == 1) {
		hanoi(1, 3, 2, disks);
		printf("How many disks?\n");
	}

	return 0;
}
