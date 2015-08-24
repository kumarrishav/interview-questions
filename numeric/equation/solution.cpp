/* Write a function that finds and prints all solutions for a^3 + b^3 = c^3 + d^3,
 * where a, b, c, and d are different and lie in the range [0, 10^3].
 *
 * Source: Careercup (Google interview)
 */

#include <iostream>
#include <cassert>
#include <map>

using namespace std;

static const unsigned max_val = 1000;

void find_solutions(void) {
	unsigned a, b, c, d;

	map<unsigned, pair<unsigned, unsigned> > values;

	for (a = 0; a <= max_val; a++)
		for (b = a+1; b <= max_val; b++)
			values[a*a*a+b*b*b] = pair<unsigned, unsigned>(a, b);

	for (c = 0; c <= max_val; c++) {
		for (d = c+1; d <= max_val; d++) {
			map<unsigned, pair<unsigned, unsigned> >::const_iterator res_it;
			res_it = values.find(c*c*c+d*d*d);
			if (res_it != values.end()) {
				a = res_it->second.first;
				b = res_it->second.second;
				if (a != c && b != d) {
					assert(a*a*a+b*b*b == c*c*c+d*d*d);
					assert(a != b && a != c && a != d);
					assert(b != c && b != d);
					assert(c != d);
					cout << a << "^3 + " << b << "^3 = " << c << "^3 + " << d << "^3" << endl;
				}
			}
		}
	}
}

int main(void) {
	find_solutions();
	return 0;
}
