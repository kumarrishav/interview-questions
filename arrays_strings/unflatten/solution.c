
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
