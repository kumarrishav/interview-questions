
#include <iostream>
#include <vector>
#include <set>

using namespace std;

/* Returns true if there exists a, b, c in arr s.t. arr[a]+arr[b]+arr[c] == k
 *
 * O(N^2 log(N)) time, O(N) space.
 *
 * With a hash table can be reduced to O(N^2) time.
 *
 */
bool k_sum(const vector<int> &arr, int k) {
	set<int> values;
	int len = arr.size();

	for (int i = len-1; i >= 0; i--) {
		if (values.find(-arr[i]) != values.end())
			return true;
		for (int j = i+1; j < len; j++)
			values.insert(arr[i]+arr[j]-k);
	}

	return false;
}

bool find_bottles(const vector<int> &bottle_sizes, int quantity) {
	return k_sum(bottle_sizes, quantity);
}

int main(void) {
	cout << "Enter number of bottles, followed by the size of each bottle, followed by the desired quantity." << endl;
	cout << "> ";

	int bottles;
	while (cin >> bottles) {
		vector<int> bottle_sizes(bottles);

		for (int i = 0; i < bottles; i++)
			cin >> bottle_sizes[i];

		int quantity;
		cin >> quantity;

		if (find_bottles(bottle_sizes, quantity))
			cout << "True" << endl;
		else
			cout << "False" << endl;

		cout << "> ";
	}

	return 0;
}
