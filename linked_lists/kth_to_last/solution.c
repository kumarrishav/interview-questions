/* Implement an algorithm to find the kth to last element of a singly linked list.
 *
 * Source: Cracking the Coding Interview, page 77, question 2.2
 */
#include <stdio.h>
#include <assert.h>

struct list_node {
	int data;
	struct list_node *next;
};

struct list_node *kth_to_last(struct list_node *head, size_t k) {
	struct list_node *ahead = head;
	size_t i;
	for (i = 0; i < k; i++) {
		if (ahead == NULL) {
			return NULL;
		}
		ahead = ahead->next;
	}

	struct list_node *behind = head;
	while (ahead != NULL) {
		behind = behind->next;
		ahead = ahead->next;
	}

	return behind;
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

static struct list_node nodes_buff[1024];
static size_t list_sz;

int main(void) {
	printf("Enter list size and k, followed by the list elements\n");

	size_t k;

	while (scanf("%zu%zu", &list_sz, &k) == 2) {
		read_list(nodes_buff, list_sz);
		printf("Received: ");
		print_list(&nodes_buff[0]);
		struct list_node *n = kth_to_last(&nodes_buff[0], k);
		if (n == NULL) {
			printf("The list size is less than %zu; please choose a valid value.\n", k);
		} else {
			printf("%zu-th to last: %d\n", k, n->data);
		}
	}

	return 0;
}
