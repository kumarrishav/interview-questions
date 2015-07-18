/* You are given an array-like data structure Listy which lacks a size method. It does, however,
 * have an elementAt(i) method that returns the element at index i in O(1) time. If i is beyond the
 * bounds of the data structure, it returns -1. (For this reason, the data structure only supports
 * positive integers). Given a Listy which contains sorted, positive integers, find the index at
 * which an element x occurs. If x occurs multiple times, you may return any index.
 *
 * Source: Cracking the Coding Interview, 6th Ed., Exercise 10.4 (page 150)
 */

#include <stdio.h>

struct listy {
	unsigned *arr;
	size_t arr_sz;
};

static long long element_at(struct listy *ly, ssize_t idx) {
	if (!(0 <= idx && idx < ly->arr_sz))
		return -1;
	return ly->arr[idx];
}

ssize_t find_element(struct listy *ly, unsigned val) {
	if (element_at(ly, 0) == -1)
		return -1; // Zero-sized list

	ssize_t l = 1;
	ssize_t r;
	long long v = element_at(ly, l);

	while (v != -1 && v <= val) {

		if (v == val) {
			// We got lucky!
			return l;
		}

		l *= 2;
		v = element_at(ly, l);
	}

	r = l;
	l /= 2;

	while (l <= r) {
		ssize_t m = l+(r-l)/2;
		v = element_at(ly, m);

		if (v == -1)
			r = m-1;
		else if (v == val)
			return m;
		else if (v < val)
			l = m+1;
		else
			r = m-1;
	}

	return -1;
}

static unsigned arr_buff[1024];
static struct listy listy_buff = { arr_buff, -1 };

int main(void) {
	printf("Enter the number of elements in the list, followed by each element, followed by the element to find.\n");
	printf("Elements must be sorted.\n");
	printf("> ");

	size_t arr_sz;
	while (scanf("%zu", &arr_sz) == 1) {
		listy_buff.arr_sz = arr_sz;

		size_t i;
		for (i = 0; i < arr_sz; i++) {
			scanf("%u", &arr_buff[i]);
		}

		unsigned n;
		scanf("%u", &n);

		ssize_t idx = find_element(&listy_buff, n);
		if (idx == -1) {
			printf("No such element.\n");
		} else {
			printf("element_at(%zd) = %u\n", idx, arr_buff[idx]);
		}

		printf("> ");
	}

	return 0;
}
