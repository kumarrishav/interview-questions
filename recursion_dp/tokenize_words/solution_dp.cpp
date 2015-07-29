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
