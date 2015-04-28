/* You have two numbers represented by a linked list, where each node contains a
 * single digit. The digits are stored in reverse order, such that the 1's digit is at the
 * head of the list. Write a function that adds the two numbers and returns the sum
 * as a linked list.
 * EXAMPLE
 * Input: (7 -> 1 -> 6) + (5 -> 9 -> 2). That is, 617 + 295.
 * Output: 2 -> 1 -> 9. That is, 912.
 *
 * Source: Cracking the Coding Interview, page 77, question 2.5
 */
#include <stdio.h>
#include <assert.h>

struct list_node {
	int data;
	struct list_node *next;
};

/* Design choice: caller provides buffer to store the result
 * This is generally a good choice in languages with manual memory management
 */
struct list_node *bigint_reversed_sum(struct list_node *h1, struct list_node *h2,
				      struct list_node buff[]) {

	struct list_node *cursor1 = h1;
	struct list_node *cursor2 = h2;
	size_t res_i = 1;
	unsigned carry = 0;
	buff[0].next = NULL;

	while (cursor1 != NULL && cursor2 != NULL) {
		int value = cursor1->data+cursor2->data;

		buff[res_i].data = (value+carry)%10;
		buff[res_i].next = NULL;
		buff[res_i-1].next = &buff[res_i];
		carry = (value+carry)/10;

		res_i++;
		cursor1 = cursor1->next;
		cursor2 = cursor2->next;
	}

	struct list_node *missing = (cursor1 != NULL ? cursor1 : cursor2);

	while (missing != NULL) {
		buff[res_i].data = (missing->data+carry)%10;
		buff[res_i].next = NULL;
		buff[res_i-1].next = &buff[res_i];
		carry = (missing->data+carry)/10;

		res_i++;
		missing = missing->next;
	}

	if (carry != 0) {
		buff[res_i].data = carry;
		buff[res_i].next = NULL;
		buff[res_i-1].next = &buff[res_i];
	}

	return buff[0].next;
}

void read_list(struct list_node nodes[], size_t list_sz) {

	assert(list_sz > 0);

	size_t i;
	for (i = 0; i < list_sz; i++) {
		scanf("%d", &nodes[i].data);
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

	printf("%d", head->data);

	head = head->next;
	while (head != NULL) {
		printf(" -> %d" ,head->data);
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
		res = bigint_reversed_sum(&operand1[0], &operand2[0], &result[0]);

		printf("\t");
		print_list(res);
		printf("\n");
	}

	return 0;
}
