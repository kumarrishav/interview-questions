/* The code below sorts a linked list using mergesort */
#include <stdio.h>

struct list_node {
	int val;
	struct list_node *next;
};

static struct list_node *find_middle(struct list_node *head) {

	if (head == NULL) {
		return NULL;
	}

	struct list_node *slow = head;
	struct list_node *fast = head->next;

	while (fast != NULL && fast->next != NULL) {
		slow = slow->next;
		fast = fast->next->next;
	}

	return slow;
}

static struct list_node *merge(struct list_node *left_head, struct list_node *right_head) {

	struct list_node sentinel_left;
	struct list_node sentinel_right;
	struct list_node *left_cursor = &sentinel_left;
	struct list_node *right_cursor = &sentinel_right;
	sentinel_left.next = left_head;
	sentinel_right.next = right_head;

	while (left_cursor->next != NULL && right_cursor->next != NULL) {
		if (left_cursor->next->val > right_cursor->next->val) {
			struct list_node *to_insert = right_cursor->next;
			right_cursor->next = right_cursor->next->next;
			to_insert->next = left_cursor->next;
			left_cursor->next = to_insert;
		}
		left_cursor = left_cursor->next;
	}

	if (left_cursor->next == NULL) {
		left_cursor->next = right_cursor->next;
	}

	return sentinel_left.next;
}

struct list_node *mergesort(struct list_node *head) {
	if (head == NULL || head->next == NULL) {
		return head;
	}

	struct list_node *head_right;
	struct list_node *middle = find_middle(head);

	head_right = middle->next;
	middle->next = NULL;

	struct list_node *sorted_left = mergesort(head);
	struct list_node *sorted_right = mergesort(head_right);

	return merge(sorted_left, sorted_right);
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

static struct list_node nodes_buff[512];

static void print_list(struct list_node *head) {
	if (head != NULL) {
		printf("%d", head->val);
		head = head->next;
	}

	while (head != NULL) {
		printf(" -> %d", head->val);
		head = head->next;
	}

	printf("\n");
}

int main(void) {
	printf("Enter the size of the list, followed by the elements.\n");
	printf("> ");

	size_t list_sz;
	while (scanf("%zu", &list_sz) == 1) {
		struct list_node head;
		struct list_node *prev = &head;

		size_t i;
		for (i = 0; i < list_sz; i++) {
			scanf("%d", &nodes_buff[i].val);
			prev->next = &nodes_buff[i];
			prev = &nodes_buff[i];
		}

		prev->next = NULL;

		struct list_node *sorted = mergesort(head.next);
		print_list(sorted);

		printf("> ");
	}

	return 0;
}
