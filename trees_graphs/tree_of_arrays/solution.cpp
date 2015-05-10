/* Suppose you have a tree where each node stores an array of words. Each node can have any
 * number of children. Here's a possible way to do this in C++:
 *
 * ~~~
 * struct tree_node {
 *	vector<string> words;
 *	vector<tree_node *> children;
 * };
 * ~~~
 *
 * Question (a)
 * - Write a method that finds and prints every node in a tree that contains a given word
 *
 * Question (b)
 * - Given a list of words, find and print every possible path that starts at the root and ends in
 *   any node, such that each word in the list is matched by a node in the path.
 *
 * Source: past interview experience
 */
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>

using namespace std;

struct tree_node {
	vector<string> words;
	vector<tree_node *> children;
};

void report_node(const tree_node *node) {
	cout << "[ ";

	if (node->words.size() > 0) {
		cout << node->words[0];
	}

	for (vector<string>::size_type word_i = 1; word_i < node->words.size(); word_i++) {
		cout << ", " << node->words[word_i];
	}

	cout << " ];";
}

/* ~~~ Question (a) ~~~ */
void nodes_with_word(const tree_node *root, const string &word) {
	vector<string>::const_iterator word_it = find(root->words.begin(), root->words.end(), word);

	if (word_it != root->words.end()) {
		report_node(root);
		cout << endl;
	}

	for (vector<tree_node*>::const_iterator node_it = root->children.begin();
	     node_it != root->children.end();
	     node_it++) {
		nodes_with_word(*node_it, word);
	}
}

/* ~~~ Question (b) ~~~ */
void print_path(vector<tree_node*> &path) {
	cout << "~~~ Start of path ~~~" << endl;
	if (path.size() > 0) {
		report_node(path[0]);
		cout << endl;
	}
	for (vector<tree_node*>::size_type node_i = 1; node_i < path.size(); node_i++) {
		cout << endl << "|" << endl << "v" << endl << endl;
		report_node(path[node_i]);
		cout << endl;
	}
	cout << "~~~ End of path ~~~" << endl << endl;
}

void find_path_aux(tree_node *root, vector<tree_node*> &path, set<string> &words_list);

void recurse_on_children(tree_node *root, vector<tree_node*> &path, set<string> &words_list) {
	for (vector<tree_node*>::const_iterator child_it = root->children.begin();
	     child_it != root->children.end();
	     child_it++) {
		find_path_aux(*child_it, path, words_list);
	}
}

void find_path_aux(tree_node *root, vector<tree_node*> &path, set<string> &words_list) {

	path.push_back(root);

	for (vector<string>::const_iterator word = root->words.begin();
	     word != root->words.end();
	     word++) {
		set<string>::const_iterator words_list_it = words_list.find(*word);
		if (words_list_it != words_list.end()) {
			words_list.erase(*word);
			if (words_list.size() == 0) {
				print_path(path);
			} else {
				recurse_on_children(root, path, words_list);
			}
			words_list.insert(*word);
		}
	}

	recurse_on_children(root, path, words_list);

	path.pop_back();

}

void find_path(tree_node *root, set<string> &words_list) {
	vector<tree_node*> path_buff;
	find_path_aux(root, path_buff, words_list);
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

static tree_node nodes[13];

int main(void) {
	nodes[0].words.push_back("hello");
	nodes[0].children.push_back(&nodes[1]);
	nodes[0].children.push_back(&nodes[2]);

	nodes[1].words.push_back("a");
	nodes[1].words.push_back("node");
	nodes[1].children.push_back(&nodes[3]);
	nodes[1].children.push_back(&nodes[4]);

	nodes[2].words.push_back("hello");
	nodes[2].words.push_back("world");
	nodes[2].children.push_back(&nodes[5]);
	nodes[2].children.push_back(&nodes[6]);

	nodes[3].words.push_back("cat");
	nodes[3].words.push_back("tac");
	nodes[3].words.push_back("god");
	nodes[3].children.push_back(&nodes[7]);

	nodes[4].words.push_back("dog");
	nodes[4].words.push_back("cat");
	nodes[4].children.push_back(&nodes[8]);
	nodes[4].children.push_back(&nodes[9]);

	nodes[5].words.push_back("cat");
	nodes[5].words.push_back("dog");
	nodes[5].children.push_back(&nodes[10]);

	nodes[6].words.push_back("nothing");
	nodes[6].words.push_back("here");

	nodes[7].words.push_back("my");
	nodes[7].words.push_back("dog");
	nodes[7].words.push_back("is");
	nodes[7].words.push_back("cute");

	nodes[8].words.push_back("cat");
	nodes[8].words.push_back("other");
	nodes[8].words.push_back("words");
	nodes[8].children.push_back(&nodes[11]);

	nodes[9].words.push_back("leaf");
	nodes[9].words.push_back("dog");

	nodes[10].words.push_back("cat");
	nodes[10].words.push_back("dog");
	nodes[10].children.push_back(&nodes[12]);

	nodes[11].words.push_back("end");
	nodes[11].words.push_back("of");
	nodes[11].words.push_back("the");
	nodes[11].words.push_back("line");

	nodes[12].words.push_back("hello");
	nodes[12].words.push_back("cat");
	nodes[12].words.push_back("dog");

	cout << "Available commands are:" << endl;
	cout << "F <word> - Find and output every node that contains the word <word>" << endl;
	cout << "P N <word1> <word2> <word3> ... <wordN> - Find and output every path that contains all of the words. Each node matches at most 1 word" << endl;
	cout << "Q - Quit" << endl;
	cout << "> ";

	char op;
	while (cin >> op) {
		if (op == 'F') {
			string word;
			cin >> word;
			nodes_with_word(&nodes[0], word);
		} else if (op == 'P') {

			set<string> words;
			set<string>::size_type words_count;

			cin >> words_count;

			for (set<string>::size_type word_i = 0; word_i < words_count; word_i++) {
				string word;
				cin >> word;
				words.insert(word);
			}
			find_path(&nodes[0], words);
		} else if (op == 'Q') {
			break;
		} else {
			cerr << "Unrecognized command: " << op << endl;
		}

		cout << "> ";
	}

	return 0;
}
