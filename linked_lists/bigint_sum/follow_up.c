/* FOLLOW UP
 * Suppose the digits are stored in forward order. Repeat the above problem.
 * EXAMPLE
 * Input: (6 -> 1 -> 7) + (2 -> 9 -> 5). That is, 617 + 295.
 * Output: 9 -> 1 -> 2. That is, 912.
 *
 * Source: Cracking the Coding Interview, page 77, question 2.5
 */
#include <assert.h>
#include <stdio.h>

struct list_node {
	int value;
	struct list_node *next;
};

struct result_wrapper {
	unsigned carry;
	struct list_node *next_head;
	size_t buff_i;
};

size_t list_length(struct list_node *head) {
	size_t res;
	for (res = 0; head != NULL; res++, head = head->next)
		; /* Intentionally left blank */
	return res;
}

struct list_node *list_prepend_zeros(struct list_node *head, struct list_node buff[],
				     size_t zeros) {
	size_t i;
	for (i = 0; i < zeros; i++) {
		struct list_node *new_node = &buff[i];
		new_node->value = 0;
		new_node->next = head;
		head = new_node;
	}

	return head;
}

// Assumes both lists have the same length
struct result_wrapper bigint_sum_aux(struct list_node *h1, struct list_node *h2,
				     struct list_node buff[]) {
	struct result_wrapper res = { 0, NULL, 0 };
	if (h1 == NULL && h2 == NULL) {
		return res;
	}

	assert(h1 != NULL && h2 != NULL);

	struct result_wrapper rest = bigint_sum_aux(h1->next, h2->next, buff);

	size_t buff_i = rest.buff_i;
	int sum_value = h1->value+h2->value+rest.carry;

	struct list_node *new_node = &buff[buff_i++];
	new_node->value = sum_value%10;
	new_node->next = rest.next_head;

	res.carry = sum_value/10;
	res.next_head = new_node;
	res.buff_i = buff_i;

	return res;
}

struct list_node *bigint_sum(struct list_node *h1, struct list_node *h2, struct list_node buff[]) {

	size_t h1_len = list_length(h1);
	size_t h2_len = list_length(h2);
	size_t buff_i = 0;

	if (h1_len != h2_len) {
		if (h1_len < h2_len) {
			buff_i = h2_len - h1_len;
			h1 = list_prepend_zeros(h1, buff, buff_i);
		} else {
			buff_i = h1_len - h2_len;
			h2 = list_prepend_zeros(h2, buff, buff_i);
		}
	}

	struct result_wrapper res = bigint_sum_aux(h1, h2, &buff[buff_i]);

	struct list_node *ret = res.next_head;
	if (res.carry != 0) {
		ret = &buff[buff_i+res.buff_i];
		ret->next = res.next_head;
		ret->value = res.carry;
	}

	return ret;
}

void read_list(struct list_node nodes[], size_t list_sz) {

	assert(list_sz > 0);

	size_t i;
	for (i = 0; i < list_sz; i++) {
		scanf("%d", &nodes[i].value);
		if (i > 0) {
			nodes[i-1].next = &nodes[i];
		}
	}
	nodes[i-1].next = NULL;
}

void print_list(struct list_node *head) {
	if (head == NULL) {
		return;
	}

	printf("%d", head->value);

	head = head->next;
	while (head != NULL) {
		printf(" -> %d" ,head->value);
		head = head->next;
	}

	printf("\n");
}

#define MAX_OPERAND_DIGITS 1024
static struct list_node operand1[MAX_OPERAND_DIGITS];
static struct list_node operand2[MAX_OPERAND_DIGITS];
static struct list_node result[MAX_OPERAND_DIGITS+2];

int main(void) {
	printf("Enter the size of operand 1, followed by the size of operand 2, followed by the "
	       "numbers in operand 1, followed by the numbers in operand 2.\n");

	size_t op1_sz, op2_sz;

	while (scanf("%zu%zu", &op1_sz, &op2_sz) == 2) {
		read_list(operand1, op1_sz);
		read_list(operand2, op2_sz);

		printf("\t");
		print_list(&operand1[0]);
		printf("+\t");
		print_list(&operand2[0]);
		printf("----------------------------------------------------\n");

		struct list_node *res;
		res = bigint_sum(&operand1[0], &operand2[0], &result[0]);

		printf("\t");
		print_list(res);
		printf("\n");
	}

	return 0;
}
