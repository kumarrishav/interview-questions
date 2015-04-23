/* A heap implementation with a sample use case (heapsort)
 * Heap sort has guaranteed running time O(N log(N))
 */
#include <stdio.h>
#include <assert.h>

#define max(a, b) ((a) > (b) ? (a) : (b))
#define MAX_HEAP_SZ 256

void swap(int arr[], size_t i, size_t j) {
	int tmp = arr[i];
	arr[i] = arr[j];
	arr[j] = tmp;
}

void max_heapify(int arr[], size_t arr_sz, size_t i) {
	size_t left = i*2+1;
	size_t right = left+1;
	size_t largest = i;

	if (left < arr_sz && arr[left] >= arr[i])
		largest = left;
	if (right < arr_sz && arr[right] >= arr[largest])
		largest = right;

	while (largest != i) {
		swap(arr, i, largest);

		i = largest;
		left = i*2+1;
		right = left+1;

		if (left < arr_sz && arr[left] >= arr[i])
			largest = left;
		if (right < arr_sz && arr[right] >= arr[largest])
			largest = right;
	}
}

void build_max_heap(int arr[], size_t arr_sz) {
	assert(arr_sz > 0);
	size_t i = (arr_sz-1)/2;

	while (i > 0) {
		max_heapify(arr, arr_sz, i);
		i--;
	}

	max_heapify(arr, arr_sz, i);
}

void heapsort(int arr[], size_t arr_sz) {
	build_max_heap(arr, arr_sz);

	while (arr_sz > 0) {
		swap(arr, 0, arr_sz-1);
		arr_sz--;
		max_heapify(arr, arr_sz, 0);
	}
}

static int heap[MAX_HEAP_SZ];
static size_t heap_sz;

static void print_array(int arr[], size_t arr_sz) {
	size_t i;
	for (i = 0; i < arr_sz; i++) {
		printf("%d ", arr[i]);
	}
	printf("\n");
}

int main(void) {
	printf("Input format: N n1 n2 n3 ...\n");
	printf("N is the array size; the rest are the elements\n");

	while (scanf("%zu", &heap_sz) == 1) {
		size_t i;
		for (i = 0; i < heap_sz; i++) {
			scanf("%d", &heap[i]);
		}
		printf("Received: ");
		print_array(heap, heap_sz);
		printf("Sorted: ");
		heapsort(heap, heap_sz);
		print_array(heap, heap_sz);
	}

	return 0;
}
