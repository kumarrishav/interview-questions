#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#define ALLOC_MIN_UNITS 1024

typedef long Align;

union header {
	struct {
		union header *next;
		union header *prev;
		size_t size;
	} s;
	Align x;
};

typedef union header Header;

static Header free_lst = { .s = { &free_lst, &free_lst, 0 } };

void *alloc_mem(size_t units);

void *malloc(size_t nbytes) {
	printf("*** Inside malloc ***\n");
	size_t units = (nbytes+sizeof(Header)-1)/sizeof(Header)+1;

	Header *block;
	for (block = free_lst.s.next;
	     block != &free_lst && block->s.size < units;
	     block = block->s.next)
		; /* Intentionally left blank */

	if (block != &free_lst) {
		if (block->s.size == units) {
			block->s.prev->s.next = block->s.next;
			block->s.next->s.prev = block->s.prev;
		} else {
			Header *new_block = block+units;
			new_block->s.size = block->s.size-units;
			new_block->s.prev = block->s.prev;
			new_block->s.next = block->s.next;

			block->s.prev->s.next = new_block;
			block->s.next->s.prev = new_block;
		}
		return block+1;
	} else {
		if (alloc_mem(units) == NULL) {
			return NULL;
		} else {
			return malloc(nbytes);
		}
	}
}

void free(void *ptr) {
	printf("*** Inside free ***\n");
	Header *to_free = ptr;
	to_free--;

	Header *right;
	for (right = free_lst.s.next;
	     right != &free_lst && ((uintptr_t) right) < ((uintptr_t) to_free);
	     right = right->s.next)
		; /* Intentionally left blank */

	to_free->s.next = right;
	to_free->s.prev = right->s.prev;

	right->s.prev = to_free;
	to_free->s.prev->s.next = to_free;

	/* Coalesce adjacent blocks */
	if (to_free+to_free->s.size == right) {
		to_free->s.size += right->s.size;
		right->s.next->s.prev = to_free;
		to_free->s.next = right->s.next;
	}
	if (to_free->s.prev+to_free->s.prev->s.size == to_free) {
		to_free->s.prev->s.size += to_free->s.size;
		to_free->s.prev->s.next = to_free->s.next;
		to_free->s.next->s.prev = to_free->s.prev;
	}
}

void *alloc_mem(size_t units) {
	size_t to_alloc = units;

	if (units < ALLOC_MIN_UNITS) {
		to_alloc = ALLOC_MIN_UNITS;
	}

	void *allocated = sbrk(to_alloc*sizeof(Header));
	if (allocated == (void *) -1) {
		return NULL;
	}

	((Header *) allocated)->s.size = units;

	free(((Header *) allocated+1));

	return allocated;
}

int main(void) {
	char *my_dynamic_buffer = malloc(27);
	strcpy(my_dynamic_buffer, "abcdefghijklmnopqrstuvwxyz");
	printf("Hello, world!\n");
	printf("%s\n", my_dynamic_buffer);
	free(my_dynamic_buffer);
	return 0;
}
