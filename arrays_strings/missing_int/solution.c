/* You are given an array A of n unique integers. Write a function that finds and returns an integer
 * that is not in A.
 *
 * EXAMPLE
 *
 * Input:  [ 0, 4, 1, 2, 100, 25 ]
 * Output: 10 (or any other number not in the array)
 *
 * Source: pramp.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

int find_missing_int(int arr[], size_t arr_sz) {
	int *buff = malloc(sizeof(arr[0])*(arr_sz+1));
	assert(buff != NULL);
	memset(buff, 0, arr_sz+1);

	size_t i;
	for (i = 0; i < arr_sz; i++)
		buff[arr[i]%(arr_sz+1)] = 1;

	for (i = 0; i < arr_sz+1 && buff[i] != 0; i++)
		; /* Intentionally left blank */

	assert(i < arr_sz+1);

	return i;
}

int main(void) {
	static int arr_buff[1024];
	size_t arr_sz;

	printf("Enter size of array, followed by the array elements.\n");
	printf("> ");

	while (scanf("%zu", &arr_sz) == 1) {
		size_t i;
		for (i = 0; i < arr_sz; i++)
			scanf("%d", &arr_buff[i]);
		printf("%d\n", find_missing_int(arr_buff, arr_sz));
		printf("> ");
	}

	return 0;
}
