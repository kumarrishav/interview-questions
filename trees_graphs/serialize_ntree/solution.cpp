/* You have an n-ary tree of integers where each node is described by the following struct:
 *
 * struct tree_node {
 *     int val;
 *     vector<tree_node *> children;
 * };
 *
 * How would you write a library to serialize and deserialize this tree? The serialize method
 * should receive a pointer to the root of the tree and return a textual string representation of
 * the tree.
 *
 * The deserialize method should receive the textual string representatio of the tree and return
 * a pointer to its root.
 *
 * Note that each node may have a different, arbitrary number of children.
 *
 * Source: Careercup (Google interview)
 */

#include <iostream>
#include <vector>
#include <queue>
#include <sstream>

using namespace std;

struct tree_node {
	int val;
	vector<tree_node *> children;
	tree_node(int v) : val(v) { }
};

/* This solution relies on the fact that an n-ary tree is just an acyclic directed graph.
 * To serialize, we perform a BFS on the tree from the root, printing the number of children
 * of a node before printing the children themselves.
 *
 * So, for this tree:
 *
 *                           1
 *                 /    /    |    \    \
 *               2     3     4     5    6
 *             / | \   |    / \       / | \
 *            7  8  9  10  11 12     13 14 15
 *
 * We serialize it as:
 *
 * 1 5 2 3 4 5 6 3 7 8 9 1 10 2 11 12 0 3 13 14 15 0 0 0 0 0 0 0 0 0
 *
 * Deserializing is very similar, but we do the opposite: we initialize the BFS queue with the first
 * node in the input, read the children count, and read the same amount of subsequent nodes,
 * enqueuing them and adding them to the unqueued node's children list at the same time.
 *
 * This solution is linear in the tree size.
 */

string serialize(tree_node *root) {
	if (root == NULL)
		return "";

	queue<tree_node*> queue;
	ostringstream oss;

	queue.push(root);
	oss << root->val;

	while (!queue.empty()) {

		tree_node *curr = queue.front();
		queue.pop();
		oss << " " << curr->children.size();

		for (vector<tree_node*>::const_iterator it = curr->children.begin();
		     it != curr->children.end();
		     it++) {
			oss << " " << (*it)->val;
			queue.push(*it);
		}
	}

	return oss.str();
}

tree_node *deserialize(const string &tree_str) {
	if (tree_str == "")
		return NULL;

	queue<tree_node*> queue;
	istringstream iss(tree_str);
	int value;
	iss >> value;

	tree_node *root = new tree_node(value);
	queue.push(root);

	while (!queue.empty()) {
		tree_node *curr = queue.front();
		queue.pop();
		int count;
		iss >> count;
		for (int i = 0; i < count; i++) {
			iss >> value;
			tree_node *new_node = new tree_node(value);
			curr->children.push_back(new_node);
			queue.push(new_node);
		}
	}

	return root;
}

static void destroy_tree(tree_node *root) {
	if (root == NULL)
		return;
	for (vector<tree_node*>::iterator it = root->children.begin();
	     it != root->children.end();
	     it++)
		destroy_tree(*it);
	delete root;
}

int main(void) {
	cout << "Enter an N-ary tree as described in the comments to deserialize and serialize it again" << endl;
	cout << "> ";

	string input;
	while (getline(cin, input)) {
		tree_node *root = deserialize(input);
		cout << serialize(root) << endl;
		destroy_tree(root);
		cout << "> ";
	}

	return 0;
}
