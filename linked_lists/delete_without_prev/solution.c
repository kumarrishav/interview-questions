/* Implement an algorithm to delete a node in the middle of a singly linked list,
 * given only access to that node.
 * EXAMPLE
 * Input: the node c from the linked list a->b->c->d->e
 * Result: nothing is returned, but the new linked list looks like a->b->d->e
 *
 * Source: Cracking the Coding Interview, page 77, question 2.3
 */
#include <stdio.h>
#include <assert.h>

struct list_node {
	int data;
	struct list_node *next;
};

// Constraints: cannot be used to remove head or tail of a list
void remove_node(struct list_node *n) {
	if (n == NULL) {
		return;
	}

	if (n->next == NULL) {
		// Oops!
		fprintf(stderr, "Fatal: can't remove last node with this method\n");
		assert(0);
	}

	n->data = n->next->data;
	n->next = n->next->next;
}

void make_list(struct list_node nodes[], size_t list_sz) {

	assert(list_sz > 0);

	size_t i;
	for (i = 0; i < list_sz; i++) {
		nodes[i].data = i;
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

static struct list_node nodes_buff[1024];
static size_t list_sz;

int main(void) {
	printf("Enter list size and node to remove (nodes go from 0 to n-1)\n");

	size_t to_remove;

	while (scanf("%zu%zu", &list_sz, &to_remove) == 2) {
		make_list(nodes_buff, list_sz);
		printf("Before removal: ");
		print_list(&nodes_buff[0]);
		remove_node(&nodes_buff[to_remove]);
		printf("After removal: ");
		print_list(&nodes_buff[0]);
	}

	return 0;
}

