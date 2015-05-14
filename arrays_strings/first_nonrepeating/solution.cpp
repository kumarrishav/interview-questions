/* Given a string, find the first non-repeating character in a single pass of the string.
 * Assume a big character set is in use, like UTF-8, which makes it unfeasible to use a
 * character frequency count table.
 * Avoid any inner loops to have an optimal runtime. Only one pass through the string is allowed.
 *
 * Source: Careercup (Yahoo interview)
 */
#include <iostream>
#include <map>

using namespace std;

struct list_node {
	string::size_type idx;
	list_node *next;
	list_node *prev;
};

static void list_init(struct list_node *head) {
	head->next = head->prev = head;
}

static void list_append(struct list_node *head, struct list_node *new_node) {
	new_node->prev = head->prev;
	new_node->next = head;
	new_node->prev->next = new_node;
	head->prev = new_node;
}

static void list_delete(struct list_node *to_del) {
	to_del->prev->next = to_del->next;
	to_del->next->prev = to_del->prev;
	// Not needed - just for safety
	to_del->next = to_del->prev = to_del;
}

static bool list_is_empty(struct list_node *head) {
	return head->next == head;
}

static void free_nodes(struct list_node *head) {
	struct list_node *curr_node = head->next;

	while (curr_node != head) {
		struct list_node *next_node = curr_node->next;
		delete curr_node;
		curr_node = next_node;
	}
}

bool first_non_repeating(const string &str, char &out) {
	map<char, list_node *> char_to_entry;
	struct list_node head;

	list_init(&head);

	for (string::size_type i = 0; i < str.size(); i++) {
		map<char, list_node*>::iterator entry_it;
		entry_it = char_to_entry.find(str[i]);
		if (entry_it != char_to_entry.end() && entry_it->second == NULL) {
			continue;
		} else if (entry_it != char_to_entry.end()) {
			list_delete(entry_it->second);
			delete entry_it->second;
			entry_it->second = NULL;
		} else {
			struct list_node *lst_entry = new list_node;
			lst_entry->idx = i;
			list_append(&head, lst_entry);
			char_to_entry[str[i]] = lst_entry;
		}
	}

	if (list_is_empty(&head)) {
		return false;
	}

	out = str[head.next->idx];
	free_nodes(&head);

	return out;
}

int main(void) {
	cout << "Enter a string to get the first non-repeating character" << endl;
	cout << "> ";

	string str;
	while (cin >> str) {
		char res;
		if (first_non_repeating(str, res)) {
			cout << res << endl;
		} else {
			cout << "There are no unique characters" << endl;
		}
		cout << "> ";
	}

	return 0;
}
