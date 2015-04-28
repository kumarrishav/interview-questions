
#include <stdio.h>
#include <assert.h>

struct list_node {
	int data;
	struct list_node *next;
};

int is_palindrome_aux(struct list_node *slow, struct list_node *fast, struct list_node **out) {
	if (fast == NULL) {
		*out = slow;
		return 1;
	}
	if (fast->next == NULL) {
		*out = slow->next;
		return 1;
	}

	struct list_node *compare_to;
	int res = is_palindrome_aux(slow->next, fast->next->next, &compare_to);

	if (!res) {
		return res;
	}

	*out = compare_to->next;
	return slow->data == compare_to->data;
}

int is_palindrome(struct list_node *head) {
	struct list_node *out;
	return is_palindrome_aux(head, head, &out);
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
	printf("Enter list size followed by the list elements\n");

	while (scanf("%zu", &list_sz) == 1) {
		read_list(nodes_buff, list_sz);
		printf("Received: ");
		print_list(&nodes_buff[0]);
		printf("Is palindrome: %s\n", is_palindrome(&nodes_buff[0]) ? "yes" : "no");
	}

	return 0;
}
