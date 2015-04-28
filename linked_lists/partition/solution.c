/* Write code to partition a linked list around a value x, such that all nodes less than
 * x come before all nodes greater than or equal to x.
 *
 * Source: Cracking the Coding Interview, page 77, question 2.4
 */
#include <stdio.h>
#include <assert.h>

struct list_node {
	int data;
	struct list_node *next;
};

struct list_node *append_to(struct list_node *tail, struct list_node *n) {
	tail->next = n;
	n->next = NULL;
	return n;
}

struct list_node *partition_list(struct list_node *head, int v) {
	struct list_node lower_head = { 0, NULL };
	struct list_node higher_head = { 0, NULL };
	struct list_node *lower_tail = &lower_head;
	struct list_node *higher_tail = &higher_head;
	struct list_node *cursor = head;

	while (cursor != NULL) {
		struct list_node *next_cursor = cursor->next;
		if (cursor->data < v) {
			lower_tail = append_to(lower_tail, cursor);
		} else {
			higher_tail = append_to(higher_tail, cursor);
		}
		cursor = next_cursor;
	}

	lower_tail->next = higher_head.next;
	return lower_head.next;
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
	printf("Enter list size and a value v to partition the list\n");

	int v;

	while (scanf("%zu%d", &list_sz, &v) == 2) {
		read_list(nodes_buff, list_sz);
		printf("Received: ");
		print_list(&nodes_buff[0]);
		struct list_node *partitioned_head = partition_list(&nodes_buff[0], v);
		printf("Partitioned: ");
		print_list(partitioned_head);
	}

	return 0;
}
