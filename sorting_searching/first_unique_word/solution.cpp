/* Write a program that takes a filename as an argument and outputs
 * the first unique word in that file.
 *
 * Source: Careercup
 */
#include <iostream>
#include <fstream>
#include <map>

using namespace std;

struct list_node {
	string word;
	struct list_node *prev, *next;
};

void init_list_head(list_node *n) {
	n->prev = n->next = n;
}

void list_append(list_node *head, list_node *n) {
	n->prev = head->prev;
	n->next = head;

	head->prev->next = n;
	head->prev = n;
}

void list_del(list_node *n) {
	n->prev->next = n->next;
	n->next->prev = n->prev;
	n->prev = n->next = n;
}

bool list_is_empty(list_node *head) {
	return head->next == head;
}

int first_unique_word(ifstream &ifs, string &out) {
	map<string, list_node *> words;
	list_node list_head;

	init_list_head(&list_head);

	string word;
	while (ifs >> word) {
		map<string, list_node *>::iterator word_it = words.find(word);

		if (word_it == words.end()) {
			list_node *new_node = new list_node;
			new_node->word = word;
			list_append(&list_head, new_node);
			words[word] = new_node;
		} else {
			if (word_it->second != NULL) {
				list_del(word_it->second);
				delete word_it->second;
				word_it->second = NULL;
			}
		}
	}

	if (list_is_empty(&list_head)) {
		return 0;
	}

	out = list_head.next->word;

	list_node *curr = list_head.next;
	while (curr != &list_head) {
		list_node *next = curr->next;
		delete curr;
		curr = next;
	}

	return 1;
}

int main(int argc, char *argv[]) {

	if (argc < 2) {
		cerr << "Usage: " << argv[0] << " filename" << endl;
		return 0;
	}

	ifstream ifs(argv[1], ifstream::in);

	if (!ifs.good()) {
		cerr << "Error opening file - please check if the file exists and if permissions are good" << endl;
		return 0;
	}

	string w;
	if (first_unique_word(ifs, w)) {
		cout << "First unique word: " << w << endl;
	} else {
		cout << "There are no unique words." << endl;
	}

	return 0;
}
