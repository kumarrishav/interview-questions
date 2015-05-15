/* Write a method to print the last K lines of an input file using C++.
 *
 * Source: Cracking the Coding Interview, page 139, question 13.1
 */
#include <iostream>
#include <cstdlib>
#include <cerrno>

using namespace std;

int main(int argc, char *argv[]) {
	if (argc < 2) {
		cerr << "Usage: " << argv[0] << " K" << endl;
		return 127;
	}

	long int K = strtol(argv[1], NULL, 10);

	if (K <= 0) {
		cerr << "Invalid value. Please use positive integers only." << endl;
		return 129;
	}

	string *buff = new string[K+1];
	size_t head = 0;
	size_t tail = 0;

	while (getline(cin, buff[tail])) {
		tail = (tail+1)%(K+1);
		if (tail == head) {
			head = (head+1)%(K+1);
		}
	}

	while (head != tail) {
		cout << buff[head] << endl;
		head = (head+1)%(K+1);
	}

	delete[] buff;

	return 0;
}
