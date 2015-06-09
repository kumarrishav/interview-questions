/* Write a function that prints a singly linked list left-to-right and right-to-left
 *
 * Source: Careercup
 */
#include <stdio.h>

struct list_node {
	int value;
	struct list_node *next;
};

static struct list_node *print_and_reverse(struct list_node *head) {
	struct list_node *prev = NULL;

	while (head != NULL) {
		printf("%d -> ", head->value);
		struct list_node *next_node = head->next;
		head->next = prev;
		prev = head;
		head = next_node;
	}

	printf("NULL\n");

	return prev;
}

void print_twice(struct list_node *head) {
	print_and_reverse(print_and_reverse(head));
}

static void print_list(struct list_node *head) {
	while (head != NULL) {
		printf("%d -> ", head->value);
		head = head->next;
	}
	printf("NULL\n");
}

static struct list_node list_nodes[1024];

int main(void) {
	printf("Enter the number of nodes and the value of each node.\n");
	printf("> ");

	size_t nodes;
	while (scanf("%zu", &nodes) == 1) {
		size_t i;

		for (i = 0; i < nodes; i++) {
			scanf("%d", &list_nodes[i].value);
			list_nodes[i].next = &list_nodes[i+1];
		}

		list_nodes[i-1].next = NULL;

		print_twice(&list_nodes[0]);

		printf("*** Checking that the list remains unchanged\n");
		print_list(&list_nodes[0]);

		printf("> ");
	}

	return 0;
}
