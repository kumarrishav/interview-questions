/* You are given two linked lists, L1 and L2, with some nodes in common.
 * That is, at some point, the lists merge into one. There is always at least
 * one node in each list before the merging spot.
 * Design an algorithm to find the merging spot.
 * EXAMPLE
 *
 * Input:
 *
 * L1 = 0 -> 1 -> 2 -> 3 -> 4 -> 5 -> 6
 *                     ^
 *                    /
 * L2 =      7 -> 8 -/
 *
 * Output: Node 3
 *
 * POSSIBLE FOLLOW UP
 * Can you do it in O(N) time using O(1) memory?
 *
 * Source: Invented by me
 */
#include <stdio.h>
#include <assert.h>

struct list_node {
	int data;
	struct list_node *next;
};

static struct list_node *find_loop_start(struct list_node *head) {
	struct list_node *slow;
	struct list_node *fast;

	slow = head->next;
	fast = head->next->next;
	while (slow != fast) {
		slow = slow->next;
		fast = fast->next->next;
	}

	slow = head;
	while (slow != fast) {
		slow = slow->next;
		fast = fast->next;
	}

	return slow;
}

struct list_node *find_merge_spot(struct list_node *h1, struct list_node *h2) {
	assert(h1 != NULL && h2 != NULL);

	struct list_node *last = h1;
	while (last->next != NULL) {
		last = last->next;
	}

	last->next = h1;
	struct list_node *merge_spot = find_loop_start(h2);
	last->next = NULL;

	return merge_spot;
}

void print_list(struct list_node *head) {
	if (head != NULL) {
		printf("%d", head->data);
		head = head->next;
	}
	while (head != NULL) {
		printf(" -> %d", head->data);
		head = head->next;
	}
}

void read_degenerate_list(struct list_node buff[], size_t entries, int nullify_last) {
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

	if (nullify_last) {
		prev->next = NULL;
	}
}

static struct list_node nodes[1024];

int main(void) {

	size_t entries1, entries2;;
	while (scanf("%zu%zu", &entries1, &entries2) == 2) {
		read_degenerate_list(nodes, entries1, 1);
		read_degenerate_list(nodes, entries2, 0);
		printf("Received: ");
		print_list(&nodes[0]);
		printf("\nReceived: ");
		print_list(&nodes[entries1]);
		printf("\nMerge spot: %d\n", find_merge_spot(&nodes[0], &nodes[entries1])->data);
	}

	return 0;
}
