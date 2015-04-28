/* Given a circular linked list, implement an algorithm which returns the node at
 * the beginning of the loop.
 * DEFINITION
 * Circular linked list: A (corrupt) linked list in which a node's next pointer points
 * to an earlier node, so as to make a loop in the linked list.
 * EXAMPLE
 * Input: A -> B -> C -> D -> E -> C (the same C as earlier)
 * Output: C
 *
 * Source: Cracking the Coding Interview, page 78, question 2.6
 */
#include <stdio.h>

struct list_node {
	int data;
	struct list_node *next;
};

struct list_node *find_loop_start(struct list_node *head) {
	struct list_node *slow;
	struct list_node *fast;

	/* slow reaches the beginning of the loop after L steps,
	 * where L is the number of hops from head to the beginning of the loop
	 * Let M = L%LOOP_SZ
	 */
	slow = head->next;
	fast = head->next->next;
	while (slow != fast) {
		slow = slow->next;
		fast = fast->next->next;
	}

	/* After slow arrives to the beginning of the loop,
	 * it meets with fast after LOOP_SZ-M steps, which means
	 * that when they meet, they are both M steps behind the loop head.
	 * So, we just need to walk L nodes. To do that, move slow back to the list's head,
	 * and keep slow and fast walking at the same rate (1 node / iteration), until they
	 * meet again
	 */
	slow = head;
	while (slow != fast) {
		slow = slow->next;
		fast = fast->next;
	}

	return slow;
}

void print_degenerate_list(struct list_node *head, size_t lim) {
	size_t i;
	for (i = 0; i < lim; head = head->next, i++) {
		printf("%d -> ", head->data);
	}
	printf("...");
}

void read_degenerate_list(struct list_node buff[], size_t entries) {
	size_t i;
	struct list_node *prev = NULL;
	for (i = 0; i < entries; i++) {
		size_t entry;
		scanf("%zu", &entry);
		buff[entry].data = entry;
		if (prev != NULL) {
			prev->next = &buff[entry];
		}
		prev = &buff[entry];
	}
}

static struct list_node nodes[1024];

int main(void) {
	size_t entries;
	while (scanf("%zu", &entries) == 1) {
		read_degenerate_list(nodes, entries);
		printf("Received: ");
		print_degenerate_list(&nodes[0], entries);
		printf("\nLoop start: %d\n", find_loop_start(&nodes[0])->data);
	}

	return 0;
}
