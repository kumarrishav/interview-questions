/* Consider the C model of arrays, where multi-dimensional arrays are linearly laid
 * out in memory and consist of arrays of arrays.
 * For example, an array a[3][4] consists of 3 contiguous arrays of 4 elements each.
 *
 * Since array dimensions are conceptual, one of the tasks a C compiler must perform is to convert,
 * or flatten, a set of dimension index values to an absolute offset. For the above example,
 * a[1][3] would be translated into offset 7, since the element a[1][3] is 7 positions away
 * from &a[0][0].
 *
 * In this question, we are interested in reversing this task. That is, given an offset i, we
 * want to reconstruct the index values for each dimension. So, in the above example, 7
 * would be converted back to a[1][3].
 *
 * You are to write a function that accomplishes this conversion - it *unflattens* the flattened
 * offset.
 *
 * You are given the array name as a string, the name of a generic indexing variable as a string,
 * the number of dimensions, D, of the array and an array of size D where D[i] is the size of
 * dimension i.
 *
 * For example, the array a[3][4] has name = "a", D = 2 (2 dimensions), D[0] = 3, and D[1] = 4.
 * The indexing variable can be any string, it is merely used for output.
 *
 * Design and implement the function that does the specified conversion.
 *
 * EXAMPLE
 * Consider that:
 * The array name is "arr"
 * The indexing variable is "i"
 * D = 2, and the dimensions array is [3, 4] (which means we have arr[3][4])
 *
 * Output = "arr[i/4][i%4]" (or any other mathematically equivalent expression)
 *
 * Explanation: any valid flattened offset "i" in "arr" is equivalent to the unflattened
 * indexing arr[i/4][i%4]. For i = 7, we have arr[7/4][7%4] = arr[1][3], as we saw earlier
 *
 * ~~~
 * void unflatten_index(const char *index_id, const char *array_name,
 *                      size_t dims, size_t dims_vals[dims],
 *                      char *out) {
 *     // Implement this
 * }
 * ~~~
 *
 * Source: Past interview experience
 */
#include <stdio.h>
#include <assert.h>

void unflatten_index(const char *index_id, const char *array_name,
		     size_t dims, size_t dims_vals[dims],
		     char *out) {
	assert(dims > 0);

	unsigned long long cum_prod[dims+1];
	cum_prod[dims] = 1;

	ssize_t i;
	for (i = dims-1; i >= 0; i--) {
		cum_prod[i] = cum_prod[i+1]*dims_vals[i];
	}

	size_t out_i = 0;
	out_i += sprintf(&out[out_i], "%s", array_name);

	for (i = 0; i < dims; i++) {
		out_i += sprintf(&out[out_i], "[(%s%%%llu)/%llu]",
				 index_id, cum_prod[i], cum_prod[i+1]);
	}
}

static char arr_name[512];
static char idx_name[512];
static char result[512];
int main(void) {
	printf("Enter an array name, an index name, the number of dimensions, and the size of each dimension\n");
	printf("> ");

	size_t dims;
	while (scanf("%s%s%zu", arr_name, idx_name, &dims) == 3) {
		size_t dims_vals[dims];
		size_t i;
		for (i = 0; i < dims; i++) {
			scanf("%zu", &dims_vals[i]);
		}

		unflatten_index(idx_name, arr_name, dims, dims_vals, result);

		printf("%s[%s] = %s\n> ", arr_name, idx_name, result);
	}

	return 0;
}
