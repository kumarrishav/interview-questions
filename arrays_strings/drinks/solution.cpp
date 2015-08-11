/* Sheldon, Leonard and Penny decide to go for drinks at Cheese Cake factory. Sheldon proposes to
 * make a game out of this. Sheldon proposes as follows:
 *
 * - Decide the amount total amount to consume, say X.
 *
 * - Pick a random number N. Get N random bottles out of the possible drinks in the factory. Each
 *   bottle contains a certain amount of liquid, for example, if N = 5, there are 5 bottles, each
 *   with a (possibly) different volume of juice.
 *
 * - Look at the random sample of size N, and if there is any combination of 3 drinks that adds up
 *   to exactly X, accept the sample, otherwise, ditch it and try again.
 *
 * They need help with the last step. Write a function that receives an array A of size N, where
 * A[i] is the volume of the i-th bottle of the random sample, and the value of X, and returns
 * true if they should accept this sample, or false if they should ditch it.
 *
 * EXAMPLE
 *
 * If N = 6, X = 22, and the volumes of the bottles are [ 1, 4, 45, 6, 10, 8 ], then the function
 * should return true, because there is a combination of 3 bottles that adds up to 22
 * (10 + 8 + 4 = 22)
 *
 * Source: Careercup
 */

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
