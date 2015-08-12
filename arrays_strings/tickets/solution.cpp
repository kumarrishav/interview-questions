/* There are M ticket booths. Each booth initially has a given number of tickets available for
 * purchase. The price of a ticket in a booth is equal to the number of tickets available
 * on that booth.
 *
 * So, for example, a booth with 9 tickets will sell the next ticket by 9 USD; there are 8 tickets
 * remaining. The next is sold by 8 USD, then by 7 USD, etc.
 *
 * We are managing the booths and want to maximize revenue. Write a function that, given the number
 * of tickets available in each booth, and a number N of tickets that will be purchased, finds the
 * maximum profit possible.
 *
 * EXAMPLE
 *
 * Suppose M = 2, and booth 1 has 1 ticket, and booth 2 has 5 tickets. If N = 4, we are interested
 * in selling 4 tickets. The best option is to sell all 4 tickets from booth 2, adding up to
 * 5 + 4 + 3 + 2 = 14 USD.
 *
 * Source: StackOverflow
 */

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int max_revenue(vector<int> &tickets, int n) {

	make_heap(tickets.begin(), tickets.end());

	int res = 0;
	for (int i = 0; i < n; i++) {
		res += tickets.front();
		pop_heap(tickets.begin(), tickets.end());
		tickets[tickets.size()-1]--;
		push_heap(tickets.begin(), tickets.end());
	}

	return res;
}


int main(void) {
	cout << "Enter number of ticket booths, followed by the number of tickets to sell." << endl;
	cout << "Then, enter how many tickets each booth has originally." << endl;
	cout << "> ";

	int booths;
	while (cin >> booths) {
		int n;
		cin >> n;
		vector<int> tickets(booths);
		for (int i = 0; i < booths; i++)
			cin >> tickets[i];
		cout << max_revenue(tickets, n) << endl;
		cout << "> ";
	}

	return 0;
}
