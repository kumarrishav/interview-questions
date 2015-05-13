
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
