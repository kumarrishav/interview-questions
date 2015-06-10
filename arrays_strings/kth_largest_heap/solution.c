/* Given a max-heap represented as an array, find and return the k-th largest element
 * without modifying the heap.
 *
 * Source: Careercup (Google interview)
 */
#include <stdio.h>

struct heap_node {
	size_t orig_i;
	int val;
};

void swap(struct heap_node arr[], size_t i, size_t j) {
	struct heap_node tmp = arr[i];
	arr[i] = arr[j];
	arr[j] = tmp;
}

void max_heap_insert(struct heap_node heap[], size_t heap_sz, int val, size_t idx) {
	heap[heap_sz].orig_i = idx;
	heap[heap_sz].val = val;

	size_t i = heap_sz;
	size_t parent = (i+1)/2-1;

	while (i != 0 && heap[i].val > heap[parent].val) {
		swap(heap, i, parent);
		i = parent;
		parent = (i+1)/2-1;
	}
}

void max_heap_extract(struct heap_node heap[], size_t heap_sz, int *val, size_t *idx) {

	*val = heap[0].val;
	*idx = heap[0].orig_i;

	if (heap_sz == 1) {
		return;
	}

	swap(heap, 0, heap_sz-1);
	heap_sz--;
	size_t curr = 0;
	size_t left = curr*2+1;
	size_t right = left+1;
	size_t largest = curr;

	if (left < heap_sz && heap[left].val > heap[largest].val) {
		largest = left;
	}
	if (right < heap_sz && heap[right].val > heap[largest].val) {
		largest = right;
	}

	while (largest != curr) {
		swap(heap, largest, curr);

		curr = largest;
		left = curr*2+1;
		right = left+1;

		if (left < heap_sz && heap[left].val > heap[largest].val) {
			largest = left;
		}
		if (right < heap_sz && heap[right].val > heap[largest].val) {
			largest = right;
		}
	}
}

int kth_largest(int max_heap[], size_t heap_sz, size_t k, int *res) {
	if (heap_sz == 0) {
		return 0;
	}

	struct heap_node aux_heap[k];
	size_t aux_heap_sz = 0;

	max_heap_insert(aux_heap, aux_heap_sz, max_heap[0], 0);
	aux_heap_sz++;
	size_t i = 1;

	while (i < k) {

		if (aux_heap_sz == 0) {
			return 0;
		}

		size_t idx;
		int val;

		max_heap_extract(aux_heap, aux_heap_sz, &val, &idx);
		aux_heap_sz--;

		size_t left = idx*2+1;
		size_t right = left+1;

		if (left < heap_sz) {
			max_heap_insert(aux_heap, aux_heap_sz, max_heap[left], left);
			aux_heap_sz++;
		}
		if (right < heap_sz) {
			max_heap_insert(aux_heap, aux_heap_sz, max_heap[right], right);
			aux_heap_sz++;
		}

		i++;		
	}

	if (aux_heap_sz == 0) {
		return 0;
	} else {
		*res = aux_heap[0].val;
		return 1;
	}
}

static int input_heap[1024];

int main(void) {
	printf("Enter number of elements in max-heap, followed by the max-heap elements, followed by k.\n");
	printf("> ");

	size_t elems;
	while (scanf("%zu", &elems) == 1) {
		size_t i;
		for (i = 0; i < elems; i++) {
			scanf("%d", &input_heap[i]);			
		}

		size_t k;
		scanf("%zu", &k);

		int res;
		if (kth_largest(input_heap, elems, k, &res)) {
			printf("%zu-th largest = %d\n", k, res);
		} else {
			printf("Invalid value of k (%zu)\n", k);
		}

		printf("> ");
	}

	return 0;
}
