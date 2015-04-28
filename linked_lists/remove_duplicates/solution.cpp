/* Write code to remove duplicates from an unsorted linked list.
 * FOLLOW UP
 * How would you solve this problem if a temporary buffer is not allowed?
 *
 * Source: Cracking the Coding Interview, question 2.1, page 77
 */
#include <cassert>
#include <iostream>
#include <map>

struct list_node {
	int data;
	struct list_node *next;
};

using namespace std;

struct list_node *remove_dups(struct list_node *head) {

	map<int, bool> seen;

	if (head == NULL) {
		return NULL;
	}

	seen[head->data] = true;

	struct list_node *prev = head;
	struct list_node *curr = head->next;

	while (curr != NULL) {
		if (seen[curr->data]) {
			prev->next = curr->next;
		} else {
			seen[curr->data] = true;
			prev = curr;
		}
		curr = curr->next;
	}

	return head;
}

struct list_node *remove_dups_no_buff(struct list_node *head) {

	if (head == NULL) {
		return NULL;
	}

	struct list_node *cursor = head;
	struct list_node *look_ahead;
	struct list_node *look_ahead_prev;

	while (cursor != NULL) {
		look_ahead_prev = cursor;
		look_ahead = cursor->next;
		while (look_ahead != NULL) {
			if (look_ahead->data == cursor->data) {
				look_ahead_prev->next = look_ahead->next;
			} else {
				look_ahead_prev = look_ahead;
			}
			look_ahead = look_ahead->next;
		}
		cursor = cursor->next;
	}

	return head;
}

void print_list(struct list_node *head) {
	if (head == NULL) {
		return;
	}

	cout << head->data;

	head = head->next;
	while (head != NULL) {
		cout << " -> " << head->data;
		head = head->next;
	}

	cout << endl;
}

static struct list_node nodes_list1[1024];
static struct list_node nodes_list2[1024];
static size_t list_size;

int main(void) {
	cout << "Enter the list size, and then each node." << endl;

	while (cin >> list_size) {

		assert(list_size > 0);

		for (size_t i = 0; i < list_size; i++) {
			cin >> nodes_list1[i].data;
			nodes_list2[i].data = nodes_list1[i].data;
			if (i > 0) {
				nodes_list1[i-1].next = &nodes_list1[i];
				nodes_list2[i-1].next = &nodes_list2[i];
			}
		}

		nodes_list1[list_size-1].next = nodes_list2[list_size-1].next = NULL;

		cout << "Received: ";
		print_list(&nodes_list1[0]);

		cout << "After removing duplicates (using buffer): ";
		print_list(remove_dups(&nodes_list1[0]));
		cout << "After removing duplicates (without buffer): ";
		print_list(remove_dups_no_buff(&nodes_list2[0]));
	}

	return 0;
}
