
#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <sstream>

using namespace std;

struct tree_node {
	int val;
	vector<tree_node*> children;
	tree_node(int v) : val(v) { }
};

unsigned largest_level(struct tree_node *root) {

	if (root == NULL)
		return 0;

	queue<tree_node*> bfs_queue;
	unsigned res = 1;
	unsigned level_count = 1;

	bfs_queue.push(root);

	while (!bfs_queue.empty()) {

		unsigned next_level = 0;
		for (unsigned i = 0; i < level_count; i++) {
			tree_node *curr = bfs_queue.front();
			bfs_queue.pop();
			for (vector<tree_node*>::iterator child_it = curr->children.begin();
			     child_it != curr->children.end();
			     child_it++) {
				bfs_queue.push(*child_it);
				next_level++;
			}
		}

		level_count = next_level;
		res = max(res, level_count);
	}

	return res;
}

struct tree_node *read_tree(const string &input) {
	if (input == "")
		return NULL;

	istringstream iss(input);
	queue<tree_node*> queue;
	int val;

	iss >> val;
	tree_node *root = new tree_node(val);
	queue.push(root);

	while (!queue.empty()) {
		tree_node *curr = queue.front();
		queue.pop();

		unsigned children;
		iss >> children;

		for (unsigned i = 0; i < children; i++) {
			iss >> val;
			tree_node *n = new tree_node(val);
			curr->children.push_back(n);
			queue.push(n);
		}
	}

	return root;
}

void destroy_tree(struct tree_node *root) {
	if (root == NULL)
		return;
	for (vector<tree_node*>::iterator child_it = root->children.begin();
	     child_it != root->children.end();
	     child_it++)
		destroy_tree(*child_it);
	delete root;
}

int main(void) {
	cout << "Enter an N-ary tree in BFS format. Enter child count for each node before entering its children" << endl;
	cout << "> ";

	string input;
	while (getline(cin, input)) {
		tree_node *root = read_tree(input);
		cout << "Largest level has " << largest_level(root) << " nodes" << endl;
		cout << "> ";
	}

	return 0;
}
