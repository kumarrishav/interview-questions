
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

static struct list_node *partition(struct list_node *head, struct list_node *pivot,
				   struct list_node **before_mid) {
	if (head == NULL) {
		return NULL;
	}

	struct list_node head_lower;
	struct list_node head_higher;
	head_lower.next = NULL;
	head_higher.next = NULL;

	struct list_node *tail_lower = &head_lower;
	struct list_node *tail_higher = &head_higher;

	while (head != NULL) {
		if (head == pivot) {
			head = head->next;
			continue;
		}
		if (head->val < pivot->val) {
			tail_lower->next = head;
			tail_lower = tail_lower->next;
		} else {
			tail_higher->next = head;
			tail_higher = tail_higher->next;
		}
		head = head->next;
	}

	tail_lower->next = pivot;
	if (tail_lower != &head_lower) {
		*before_mid = tail_lower;
	} else {
		*before_mid = NULL;
	}
	tail_lower = tail_lower->next;

	tail_higher->next = NULL;
	tail_lower->next = head_higher.next;

	return head_lower.next;
}
static void print_list(struct list_node *head);
struct list_node *quicksort(struct list_node *head) {
	if (head == NULL || head->next == NULL) {
		return head;
	}

	struct list_node *middle = find_middle(head);
	//printf("middle = %d\n", middle->val);
	struct list_node *before_mid;
	struct list_node *head_partitioned = partition(head, middle, &before_mid);

	/*printf("Partitioned list:\n");
	print_list(head_partitioned);
	printf("before_mid = ");
	if (before_mid == NULL) {
		printf("NULL\n");
	} else {
		printf("%d\n", before_mid->val);
		}*/

	middle->next = quicksort(middle->next);
	struct list_node *new_head = middle;

	if (before_mid != NULL) {
		before_mid->next = NULL;
		struct list_node *left = quicksort(head_partitioned);
		new_head = left;
		while (left->next != NULL) {
			left = left->next;
		}
		left->next = middle;
	}

	return new_head;

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

		struct list_node *sorted = quicksort(head.next);
		print_list(sorted);

		printf("> ");
	}

	return 0;
}
