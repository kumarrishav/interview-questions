/* In the classic problem of the Towers of Hanoi, you have 3 towers and N disks of
 * different sizes which can slide onto any tower. The puzzle starts with disks sorted
 * in ascending order of size from top to bottom (i.e., each disk sits on top of an
 * even larger one). You have the following constraints:
 * (1) Only one disk can be moved at a time.
 * (2) A disk is slid off the top of one tower onto the next tower.
 * (3) A disk can only be placed on top of a larger disk.
 * Write a program to move the disks from the first toweer to the last using stacks.
 *
 * Source: Cracking the Coding Interview, page 81, question 3.4
 */
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
