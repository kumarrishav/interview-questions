/* Write a method to print every possible ways of parenthesizing an expression.
 * Assume that each number in the expression is a 1 digit number.
 *
 * EXAMPLE:
 * Input: 1+2*3/5
 * Output:
 *   1+(2*(3/5))
 *   1+((2*3)/5)
 *   (1+2)*(3/5)
 *   (1+(2*3))/5
 *   ((1+2)*3)/5
 *
 */
#include <iostream>
#include <sstream>
#include <cassert>
#include <vector>

using namespace std;

static bool is_operator(char c) {
	return c == '+' || c == '-' || c == '/' || c == '*';
}

vector<string> parenthesize_aux(const string &expr, string::size_type begin, string::size_type end)
{
	vector<string> res;

	if (begin == end) {
		res.push_back(string(1, expr[begin]));
		return res;
	}

	for (string::size_type i = begin; i <= end; i++) {
		if (!is_operator(expr[i])) {
			continue;
		}

		assert(i > begin);
		assert(i < end);

		vector<string> left = parenthesize_aux(expr, begin, i-1);
		vector<string> right = parenthesize_aux(expr, i+1, end);

		for (vector<string>::const_iterator str_l = left.begin(); str_l != left.end(); str_l++) {
			for (vector<string>::const_iterator str_r = right.begin(); str_r != right.end(); str_r++) {
				ostringstream oss;
				oss << "(" << *str_l << expr[i] << *str_r << ")";
				res.push_back(oss.str());
			}
		}
	}

	return res;
}

void parenthesize(const string &expr)
{
	if (expr.size() == 0) {
		return;
	}

	if (expr.size() == 1) {
		cout << expr << endl;
		cout << "Total: 1" << endl;
		return;
	}

	assert(expr.size() > 2);

	vector<string> combinations = parenthesize_aux(expr, 0, expr.size()-1);
	for (vector<string>::const_iterator exp = combinations.begin(); exp != combinations.end(); exp++) {
		cout << exp->substr(1, exp->size()-2) << endl;
	}

	cout << "Total: " << combinations.size() << endl;

}

int main(void) {
	cout << "Enter an expression to see every possible way to parenthesize it" << endl;
	cout << "Note: only 1-digit numbers are allowed." << endl;
	cout << "> ";

	string expr;
	while (cin >> expr) {
		parenthesize(expr);
		cout << "> ";
	}

	return 0;
}
