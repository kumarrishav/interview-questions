/* You are given two streams `s_a` and `s_b` of digits. Each stream represents an integer `a` and
 * `b` from least significant to most significant digit. For example, 2048 is represented in the
 * stream as 8, 4, 0, 2.
 *
 * Write a function that subtracts the two integers in each stream, and returns the result as a
 * string. You are not allowed to buffer the contents of any stream, i.e., you may access only a
 * single digit on each stream at a time. Reading a digit from the stream consumes it, and any
 * future read operations will return the next digit on the stream.
 *
 * Assume that `a` >= `b`.
 *
 * EXAMPLE
 *
 * Input:
 *
 * s_a = 8 4 0 2
 * s_b = 4 2 0 1
 *
 * Output:
 *
 * "1024"
 *
 * Source: Careercup (Google interview)
 */
#include <iostream>
#include <istream>
#include <cstdio>
#include <algorithm>
#include <sstream>

using namespace std;

string stream_sub(istream &s_a, istream &s_b) {
	int c_a, c_b;
	int carry = 0;
	string res;

	while ((c_a = s_a.get()) != EOF && (c_b = s_b.get()) != EOF) {
		int op_a = c_a-'0';
		int op_b = c_b-'0';
		int next_carry = 0;

		if (op_a < op_b+carry) {
			op_a += 10;
		        next_carry = 1;
		}

		res.push_back(op_a-(op_b+carry)+'0');
		carry = next_carry;
	}

	// Assumption: s_a >= s_b

	while (c_a != EOF) {
		res.push_back(c_a-carry);
		carry = 0;
		c_a = s_a.get();
	}

	reverse(res.begin(), res.end());

	return res;
}

int main(void) {
	cout << "Enter the two operands (space separated). Each operand will be transformed into a characters stream" << endl;
	cout << "> ";

	string op1, op2;

	while (cin >> op1 >> op2) {
		istringstream iss_op1(op1);
		istringstream iss_op2(op2);

		cout << stream_sub(iss_op1, iss_op2) << endl;
		cout << "> ";

	}

	return 0;
}
