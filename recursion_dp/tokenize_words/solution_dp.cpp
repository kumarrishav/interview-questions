/* A fancy dynamic programming solution. This is the most elegant solution and it is O(n^2).
 * Sidenote: it is not really O(n^2) because substr() and string concatenation is not constant.
 *
 * However, if it's really necessary, we could squeeze out a little bit more performance by
 * using something similar to Java's StringBuffer to build the intermediate strings. Or instead
 * each DP table entry could store a vector of pairs (i, j) with the start index of each word
 * and its length. This would avoid string concatenation and substr() altogether. It would be nice
 * to mention it in the interview, but it is probably not necessary to implement it.
 *
 * It is often easier to analyze the runtime of dynamic programming solutions by analyzing the
 * equivalent bottom-up approach. Furthermore, developing a bottom-up solution allows us to avoid
 * recursion, which eliminates the possibility of stack overflow. This is one advantage over
 * memoization.
 *
 * Building the bottom-up rule may not be easy, but as everything else in life, practice makes
 * perfect!
 *
 * This is a very elegant solution.
 *
 * Note: invalid entries in the dp table are marked as " " (a single space). This is because a
 * solution is never a single space, so we choose to use that as a synonym of "no solution".
 *
 */

#include <iostream>
#include <vector>

using namespace std;

bool dict_find(const string &word);

bool tokenize_words(const string &str, string &out) {
	vector<string> dp(str.size()+1, " ");
	dp[str.size()] = "";

	int len = str.size();
	for (int i = len-1; i >= 0; i--) {
		for (int j = i+1; j < len+1; j++) {
			string w = str.substr(i, j-i);
			if (dict_find(w) && dp[j] != " ")
				dp[i] = w + " " + dp[j];
		}
	}

	if (dp[0] != " ") {
		out = dp[0];
		// No trailing whitespace
		if (out.size() > 0)
			out.erase(out.end()-1);
		return true;
	}

	return false;
}

int main(void) {
	cout << "Enter a string to tokenize into words" << endl;
	cout << "> ";

	string str;
	while (cin >> str) {
		string res;
		if (tokenize_words(str, res))
			cout << res << endl;
		else
			cout << "Unsolvable" << endl;
		cout << "> ";
	}

	return 0;
}

static string dictionary[] = {
	"apple",
	"pie",
	"cat",
	"a",
	"is",
	"the",
	"dinner",
	"good",
	"bad",
	"with",
	"and"
};

bool dict_find(const string &word) {
	for (size_t i = 0; i < sizeof(dictionary)/sizeof(dictionary[0]); i++)
		if (dictionary[i] == word)
			return true;
	return false;
}
