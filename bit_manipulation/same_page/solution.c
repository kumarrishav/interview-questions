/* Given two memory addresses, determine if they are in the same page.
 * Assume one page is 4096 bytes
 *
 * Source: past interview experience
 */
#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#define PAGE_SIZE 4096
#define PAGE_SIZE_MASK (~((uintptr_t) (PAGE_SIZE-1)))

int same_page(void *ptr1, void *ptr2) {
	return (((uintptr_t) ptr1) & PAGE_SIZE_MASK) == (((uintptr_t) ptr2) & PAGE_SIZE_MASK);
}

int main(void) {
	printf("Page size = %d; Pointers are %zu bits\n", PAGE_SIZE, sizeof(void *)*CHAR_BIT);
	printf("Enter 2 memory addresses to see if they are on the same memory page\n");
	printf("> ");

	void *ptr1, *ptr2;
	while (scanf("%p%p", &ptr1, &ptr2) == 2) {
		if (same_page(ptr1, ptr2)) {
			printf("They are on the same page\n");
		} else {
			printf("They are not on the same page\n");
		}
		printf("> ");
	}

	return 0;
}
