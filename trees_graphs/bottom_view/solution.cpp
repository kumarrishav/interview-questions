/* Consider a pair of nodes x and y in a binary tree, where y is the parent of x.
 * We define the horizontal distance of x, hd(x), as follows:
 *
 * hd(x) = hd(y)-1 if x is the left child of y
 * hd(x) = hd(y)+1 if x is the right child of y
 *
 * The bottom view of a binary tree consists of a set of nodes. A node x is in the bottom view set
 * if (and only if) there is no other node y deeper than x such that hd(y) = hd(x). In other words,
 * x is the deepest node in the tree with the horizontal distance hd(x).
 *
 * Write a function that finds and prints the bottom view of a binary tree. Nodes should be printed
 * as if the tree is traversed left to right, level by level.
 *
 * Source: Careercup
 */
#include <iostream>
#include <map>
#include <vector>
#include <queue>
#include <utility>

using namespace std;

struct tree_node {
	int val;
	struct tree_node *left;
	struct tree_node *right;
};

struct node_wrapper {
	tree_node *node;
	int hdistance;
	unsigned level;

	node_wrapper(tree_node *n, int hd, unsigned l) : node(n), hdistance(hd), level(l) { }
};

static void print_bottom_view(const map<int, pair<unsigned, vector<tree_node *> > > &bt_view) {
	for (map<int, pair<unsigned, vector<tree_node*> > >::const_iterator bt_it = bt_view.begin();
	     bt_it != bt_view.end();
	     bt_it++) {
		for (vector<tree_node *>::const_iterator node_it = bt_it->second.second.begin();
		     node_it != bt_it->second.second.end();
		     node_it++) {
			cout << (*node_it)->val << " ";
		}
	}
	cout << endl;
}

void bottom_view(tree_node *root) {
	map<int, pair<unsigned, vector<tree_node *> > > bt_view;
	queue<node_wrapper> bfs_queue;

	bfs_queue.push(node_wrapper(root, 0, 1));

	while (!bfs_queue.empty()) {
		node_wrapper nw = bfs_queue.front();
		bfs_queue.pop();

		map<int, pair<unsigned, vector<tree_node *> > >::iterator bt_view_it;
		bt_view_it = bt_view.find(nw.hdistance);

		if (bt_view_it == bt_view.end() || bt_view_it->second.first < nw.level) {
			vector<tree_node *> nodes_in_level;
			nodes_in_level.push_back(nw.node);
			bt_view[nw.hdistance] = make_pair(nw.level, nodes_in_level);
		} else if (bt_view_it->second.first == nw.level) {
			bt_view_it->second.second.push_back(nw.node);
		}

		if (nw.node->left != NULL) {
			bfs_queue.push(node_wrapper(nw.node->left, nw.hdistance-1, nw.level+1));
		}
		if (nw.node->right != NULL) {
			bfs_queue.push(node_wrapper(nw.node->right, nw.hdistance+1, nw.level+1));
		}
	}

	print_bottom_view(bt_view);
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

static map<int, tree_node *> nodes;
static tree_node *get_node(int v) {
	map<int, tree_node *>::iterator node_it = nodes.find(v);

	if (node_it == nodes.end()) {
		tree_node *n = new tree_node;
		n->val = v;
		n->left = n->right = NULL;
		nodes[v] = n;
	}

	return nodes[v];
}

static void destroy_tree(tree_node *root) {
	if (root == NULL) {
		return;
	}
	destroy_tree(root->left);
	destroy_tree(root->right);
	delete root;
}

int main(void) {
	cout << "Enter the number of nodes in the tree, followed by each node. Each node is a line of the format:" << endl;
	cout << "<nodeID> <left child> <right child> (-1 means no child)" << endl;
	cout << "> ";

	map<int, tree_node *>::size_type nodes_count;
	tree_node *root = NULL;

	while (cin >> nodes_count) {
		for (map<int, tree_node *>::size_type i = 0; i < nodes_count; i++) {
			int node_id;
			cin >> node_id;
			tree_node *node = get_node(node_id);

			if (root == NULL) {
				root = node;
			}

			int l, r;
			cin >> l >> r;
			if (l != -1) {
				node->left = get_node(l);
			}
			if (r != -1) {
				node->right = get_node(r);
			}
		}

		bottom_view(root);
		destroy_tree(root);
		nodes.clear();
		root = NULL;

		cout << "> ";
	}

	return 0;
}
