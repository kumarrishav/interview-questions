
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

	while (element_at(ly, l) != -1)
		l *= 2;

	r = l;
	l = 0;

	while (l <= r) {
		ssize_t m = l+(r-l)/2;
		long long v = element_at(ly, m);

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
