/* You are given an array with the result of the in-order traversal of a tree.
 * Write a method that generates every tree that yields the same array
 * for an in-order traversal.
 *
 * Source: Careercup (Google interview)
 */
#include <memory>
#include <vector>
#include <iostream>

using namespace std;

struct tree_node {
	int val;
	shared_ptr<tree_node> left;
	shared_ptr<tree_node> right;
};

static vector<shared_ptr<tree_node> > generate_trees_aux(const vector<int> &inorder,
							 vector<int>::size_type begin, vector<int>::size_type end) {
	vector<shared_ptr<tree_node> > res;

	if (begin >= end) {
		res.push_back(shared_ptr<tree_node>());
		return res;
	}

	for (vector<int>::size_type i = begin; i < end; i++) {
		vector<shared_ptr<tree_node> > left = generate_trees_aux(inorder, begin, i);
		vector<shared_ptr<tree_node> > right = generate_trees_aux(inorder, i+1, end);

		for (vector<shared_ptr<tree_node> >::const_iterator left_it = left.begin();
		     left_it != left.end();
		     left_it++) {
			for (vector<shared_ptr<tree_node> >::const_iterator right_it = right.begin();
			     right_it != right.end();
			     right_it++) {

				tree_node *root = new tree_node;
				root->val = inorder[i];
				root->left = *left_it;
				root->right = *right_it;

				res.push_back(shared_ptr<tree_node>(root));
			}
		}
	}

	return res;
}

vector<shared_ptr<tree_node> > generate_trees(const vector<int> &inorder) {
	return generate_trees_aux(inorder, 0, inorder.size());
}

static void print_tree(shared_ptr<tree_node> root) {
	if (!root) {
		return;
	}

	cout << root->val << " -> ";
	if (root->left) {
		cout << root->left->val;
	} else {
		cout << "-1";
	}
	cout << " ";
	if (root->right) {
		cout << root->right->val;
	} else {
		cout << "-1";
	}
	cout << endl;
	print_tree(root->left);
	print_tree(root->right);
}

static void print_trees(const vector<shared_ptr<tree_node> > &trees) {
	for (vector<shared_ptr<tree_node> >::const_iterator tree_it = trees.begin();
	     tree_it != trees.end();
	     tree_it++) {
		cout << "~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
		print_tree(*tree_it);
	}
}

int main(void) {
	cout << "Enter size of tree, followed by the in-order traversal (space separated integers)" << endl;
	cout << "To generate every tree that yields the specified in-order sequence" << endl;
	cout << "> ";

	vector<int>::size_type tree_sz;
	while (cin >> tree_sz) {
		
		vector<int> inorder;
		for (vector<int>::size_type i = 0; i < tree_sz; i++) {
			int value;
			cin >> value;
			inorder.push_back(value);
		}

		vector<shared_ptr<tree_node> > all_trees = generate_trees(inorder);

		cout << "Printing trees" << endl;
		cout << "Format: node -> left_child right_child (-1 means no child)" << endl;

		print_trees(all_trees);

		cout << "> ";
	}
}
