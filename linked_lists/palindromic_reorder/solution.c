/* We define a palindromic reordering of a list as follows:
 *
 * For a list of size N, palindromically reordering that list will cause node i to become
 * the predecessor of node N-i-1, 0 <= i <= N/2, and node N-i-1 becomes the predecessor of i+1.
 *
 * For example, the list 1 -> 2 -> 3 -> 4 -> 5 becomes 1 -> 5 -> 2 -> 4 -> 3.
 * The list 1 -> 2 -> 3 -> 4 becomes 1 -> 4 -> 2 -> 3.
 *
 * In other words, the first node is paired with the last node, the second node is paired with the
 * second to last, the third node is paired with the third to last, and so forth.
 *
 * Design and implement an algorithm that, given the head of a list of unknown size, palindromically
 * reorders the list.
 *
 * Source: Stackoverflow
 */
#include <stdio.h>

struct list_node {
	int value;
	struct list_node *next;
};

static struct list_node *palindromic_reorder_aux(struct list_node *slow, struct list_node *fast) {
	if (fast->next == NULL) {
		// Odd
		struct list_node *saved = slow->next;
		slow->next = NULL;
		return saved;
	} else if (fast->next->next == NULL) {
		// Even
		slow = slow->next;
		struct list_node *saved = slow->next;
		slow->next = NULL;
		return saved;
	}

	struct list_node *my_next = palindromic_reorder_aux(slow->next, fast->next->next);
	struct list_node *saved = my_next->next;
	my_next->next = slow->next;
	slow->next = my_next;

	return saved;
}

struct list_node *palindromic_reorder(struct list_node *head) {
	if (head == NULL) {
		return NULL;
	}

	palindromic_reorder_aux(head, head);

	return head;
}

static void print_list(struct list_node *head) {
	while (head != NULL) {
		printf("%d -> ", head->value);
		head = head->next;
	}
	printf("NULL\n");
}

static struct list_node nodes_buff[512];

int main(void) {

	printf("Enter size of list, followed by each element\n");
	printf("> ");

	size_t list_sz;
	while (scanf("%zu", &list_sz) == 1) {
		struct list_node sentinel;
		struct list_node *prev = &sentinel;

		size_t i;
		for (i = 0; i < list_sz; i++) {
			scanf("%d", &nodes_buff[i].value);
			prev->next = &nodes_buff[i];
			prev = &nodes_buff[i];
		}

		prev->next = NULL;

		printf("Input:\n");
		print_list(sentinel.next);
		printf("Result:\n");
		struct list_node *new_head = palindromic_reorder(sentinel.next);
		print_list(new_head);
		printf("> ");
	}

	return 0;
}
