
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
		cout << max_revenue(tickets, n << endl;
		cout << "> ";
	}

	return 0;
}
