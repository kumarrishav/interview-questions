
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <climits>
#include <ctime>

#define M 5000
#define N 5000
#define Q 1000

using namespace std;

static int row_buff[N];
void generate_row(int min, int max) {
	for (size_t i = 0; i < N; i++) {
		row_buff[i] = min+rand()%(max-min+1);
	}
	sort(row_buff, row_buff+N);
	for (size_t i = 0; i < N; i++) {
		cout << row_buff[i] << " ";
	}
	cout << endl;
}

int main(void) {
	srand(time(NULL));
	int row_quota = INT_MAX/M;
	cout << M << " " << N << endl;
	for (int i = 0; i < M; i++) {
		generate_row(i*row_quota, (i+1)*row_quota-1);
	}

	cout << endl;

	for (int i = 0; i < Q; i++) {
		cout << rand() << endl;
	}

	return 0;
}
