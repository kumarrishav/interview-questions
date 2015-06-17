/* You have an array of `N` linked lists where `array[i]` holds a pointer to the head of list `i`.
 *
 * Design and implement an algorithm that for each list `i` finds and prints the list head of every
 * list `j` in `array[i+1..N]` for which the intersection of list `array[i]` with list `array[j]` is not empty.
 *
 * EXAMPLE
 *
 * If the input consists of an array with these lists:
 *
 * array[0] = 1 -> 2 -> 3 -> 4 -> 5
 * array[1] = 6 -> 7 -> 8 -> 4 -> 5
 * array[2] = 8 -> 9 -> 10 -> 11 -> 12
 * array[3] = 13 -> 14 -> 15 -> 12
 * array[4] = 16 -> 17 -> 18
 *
 * Then the output is:
 *
 * 1 6
 * 6 8
 * 8 13
 * 13
 * 16
 *
 * Explanation:
 *
 * The list that starts with 1 (at array[0]), when intersected with the list at array[1],
 * yields a non-empty set. Furthermore, it's the only list with index greater than 0 on which
 * this happens. So the result for list 0 is the line "1 6".
 *
 * The list at array[1] (head = 6) has a non-empty intersection with the list at array[2]. It
 * is the only list with a non-empty intersection. So the result for list 1 is the line "6 8".
 *
 * A similar thing happens with the list at array[2], generating the line "8 13".
 *
 * The list at array[3] does not have any non-empty intersection with any list at array[4..N-1],
 * so the algorithm just prints the list head (13).
 *
 * A similar thing happens with the list at array[4].
 *
 * Source: Careercup (Amazon interview)
 */
#include <iostream>
#include <vector>
#include <map>
#include <set>

using namespace std;

struct list_node {
	int value;
	struct list_node *next;
};

void print_intersections(const vector<list_node *> &lists) {
	typedef vector<list_node*>::size_type size_type;
	map<int, set<size_type> > val_to_lst;

	for (size_type i = 0; i < lists.size(); i++) {
		for (list_node *curr = lists[i]; curr != NULL; curr = curr->next) {
			val_to_lst[curr->value].insert(i);
		}
	}

	for (size_type i = 0; i < lists.size(); i++) {
		set<size_type> lists_out;
		for (list_node *curr = lists[i]; curr != NULL; curr = curr->next) {
			lists_out.insert(val_to_lst[curr->value].find(i), val_to_lst[curr->value].end());
		}
		for (set<size_type>::const_iterator it = lists_out.begin(); it != lists_out.end(); it++) {
			cout << lists[*it]->value << " ";
		}
		cout << endl;
	}
}

static void print_input(const vector<list_node*> &lists) {
	for (vector<list_node*>::const_iterator it = lists.begin();
	     it != lists.end();
	     it++) {
		for (list_node *curr = *it; curr != NULL; curr = curr->next) {
			cout << curr->value << " -> ";
		}
		cout << "NULL" << endl;
	}
}

static struct list_node nodes_buff[1024];

int main(void) {
	cout << "Enter the number of lists." << endl;
	cout << "Then, for each list, enter how many elements are in the list, followed by each element" << endl;

	vector<list_node *> lists;
	vector<list_node *>::size_type lists_count;
	cin >> lists_count;

	size_t nodes_i = 0;

	for (vector<list_node *>::size_type i = 0; i < lists_count; i++) {
		struct list_node list_head;
		struct list_node *prev = &list_head;

		size_t list_sz;
		cin >> list_sz;

		for (size_t i = 0; i < list_sz; i++) {
			cin >> nodes_buff[nodes_i].value;
			prev->next = &nodes_buff[nodes_i];
			prev = &nodes_buff[nodes_i];
			nodes_i++;
		}

		prev->next = NULL;
		lists.push_back(list_head.next);

	}

	cout << endl;
	cout << "Input:" << endl;
	print_input(lists);
	cout << endl;
	cout << "Results:" << endl;
	print_intersections(lists);

	return 0;
}
