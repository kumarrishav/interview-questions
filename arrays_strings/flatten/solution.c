/* Consider the C model of arrays, where multi-dimensional arrays are linearly laid
 * out in memory and consist of arrays of arrays.
 * For example, an array a[3][4] consists of 3 contiguous arrays of 4 elements each.
 *
 * Since array dimensions are conceptual, one of the tasks a C compiler must perform is to convert,
 * or flatten, a set of dimension index values to an absolute offset. For the above example,
 * a[1][3] would be translated into offset 7, since the element a[1][3] is 7 positions away
 * from &a[0][0].
 *
 * You are to write a function that accomplishes this conversion - it derives the formula to flatten
 * a set of dimension indexing values into a single value.
 *
 * You are given the array name as a string, the number of dimensions, the name of each dimension
 * indexing variable and the size of each dimension. Output the string that corresponds to the
 * equivalent offset indexing.
 *
 * EXAMPLE
 * Consider that:
 * The array name is "arr"
 * The array has 2 dimensions
 * The indexing variables are "i" and "j" (so we want to convert the form arr[i][j])
 * The dimensions sizes are 3 and 4
 *
 * Output: "arr[i*4+j]" (or any mathematically equivalent expression)
 *         This means that the absolute offset of arr[i][j] is i*4+j
 *
 * Source: Invented (the opposite question of the unflatten question)
 */
#include <stdio.h>
#include <string.h>
#include <assert.h>

void flatten_index(const char *array_name,
		   size_t dims, const char *const idx_names[dims], size_t dims_vals[dims],
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
	out_i += sprintf(&out[out_i], "[%s*%llu", idx_names[0], cum_prod[1]);

	for (i = 1; i < dims; i++) {
		out_i += sprintf(&out[out_i], "+%s*%llu", idx_names[i], cum_prod[i+1]);
	}

	out[out_i++] = ']';
	out[out_i++] = '\0';
}

static char arr_name[512];
static char idx_names_buff[512];
static const char *idx_names[512];
static char result[512];
int main(void) {
	printf("Enter an array name, the number of dimensions, the size of each dimension, and\n"
	       "the name of each dimension index variable\n");
	printf("> ");

	size_t dims;
	while (scanf("%s%zu", arr_name, &dims) == 2) {
		size_t dims_vals[dims];
		size_t i;
		for (i = 0; i < dims; i++) {
			scanf("%zu", &dims_vals[i]);
		}

		size_t idx_offset = 0;
		for (i = 0; i < dims; i++) {
			scanf("%s", idx_names_buff+idx_offset);
			idx_names[i] = idx_names_buff+idx_offset;
			idx_offset += strlen(idx_names_buff+idx_offset)+1;
		}

		flatten_index(arr_name, dims, idx_names, dims_vals, result);

		printf("%s", arr_name);
		for (i = 0; i < dims; i++) {
			printf("[%s]", idx_names[i]);
		}
		printf(" = %s\n> ", result);

	}

	return 0;
}
