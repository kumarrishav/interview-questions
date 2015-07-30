
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
