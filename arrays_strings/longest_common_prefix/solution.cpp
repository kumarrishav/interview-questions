/* Find the longest common prefix in a list of sentences.
 *
 * EXAMPLE
 *
 * Input:
 *    "I love all dogs"
 *    "I love cats"
 *
 * Output: "I love "
 *
 * Source: Careercup (Google interview)
 */
#include <climits>
#include <cstring>
#include <cassert>
#include <iostream>
#include <vector>

using namespace std;

struct trie_node {
	int entries;
	struct trie_node *children[UCHAR_MAX+1];

	trie_node(void) : entries(0) {
		memset(children, 0, sizeof(children));
	}
};

struct trie {
	int total_inserts;
	string lp;
	struct trie_node root;

private:

	void destroy_trie(struct trie_node *root) {
		if (root == NULL) {
			return;
		}
		for (size_t i = 0; i < sizeof(root->children)/sizeof(root->children[0]); i++) {
			destroy_trie(root->children[i]);
		}
		delete root;
	}

	bool insert_internal(const string &str, string::size_type str_i, trie_node *curr_node) {
		curr_node->entries++;

		if (str_i == str.size()) {
			if (curr_node->entries == total_inserts) {
				lp = str;
				return true;
			}
			return false;
		}

		trie_node *next = curr_node->children[(unsigned char) str[str_i]];
		if (next == NULL) {
			next = new trie_node;
			curr_node->children[(unsigned char) str[str_i]] = next;
		}

		bool ret = insert_internal(str, str_i+1, next);

		if (!ret && curr_node->entries == total_inserts) {
			lp = str.substr(0, str_i);
			ret = true;
		}

		return ret;
	}

public:
	trie(void) : total_inserts(0) { }
	~trie(void) {
		for (size_t i = 0; i < sizeof(root.children)/sizeof(root.children[0]); i++) {
			destroy_trie(root.children[i]);
		}
	}

	void insert(const string &str) {
		total_inserts++;
		bool res = insert_internal(str, 0, &root);
		assert(res);
	}

	string lcp(void) {
		return lp;
	}

};

string sentences_lcp(const vector<string> &sentences) {
	trie t;
	for (vector<string>::const_iterator it = sentences.begin(); it != sentences.end(); it++) {
		t.insert(*it);
	}
	return t.lcp();
}

int main(void) {
	cout << "Enter the number of lines with sentences, followed by each sentence" << endl;
	cout << "> ";

	vector<string>::size_type sentences_sz;
	while (cin >> sentences_sz) {
		vector<string> sentences(sentences_sz);

		// Ignore trailing newline from previous input
		getline(cin, sentences[0]);

		for (vector<string>::size_type i = 0; i < sentences_sz; i++) {
			getline(cin, sentences[i]);
		}

		string lcp = sentences_lcp(sentences);

		cout << "LCP = " << lcp << endl;
		cout << "> ";
	}

	return 0;
}
