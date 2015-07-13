/* Write a function to print the unique rows of a matrix
 *
 * Source: Careercup (Amazon interview)
 */

#include <iostream>
#include <algorithm>
#include <vector>

/* We could simply sort the matrix using the library sorting routine
 *
 * However, this is expensive because traditional sorting compares elements
 * atomically, and elements here are entire rows, which means that in an
 * M x N matrix each comparison is O(N).
 *
 * Instead, we perform radix sort column by column, sorting each column
 * using the library sort. This way, comparisons will still be O(1)
 *
 */

using namespace std;

void print_row(const vector<int> &row) {
	for (vector<int>::size_type i = 0; i < row.size(); i++)
		cout << row[i] << " ";
	cout << endl;
}

static vector<int>::size_type curr_col;
bool comparator(const vector<int> &row_a, const vector<int> &row_b) {
	return row_a[curr_col] < row_b[curr_col];
}

void sort_matrix(vector<vector<int> > &matrix, unsigned row_begin, unsigned rows, unsigned col_i) {
	if (col_i == matrix[0].size()) {
		if (rows == 1) {
			print_row(matrix[row_begin]);
		}
		return;
	}

	curr_col = col_i;
	sort(matrix.begin()+row_begin, matrix.begin()+row_begin+rows, comparator);

	unsigned r = row_begin;
	while (r < row_begin+rows) {
		unsigned i = r+1;

		while (i < row_begin+rows && matrix[i][col_i] == matrix[r][col_i]) {
			i++;
		}

		sort_matrix(matrix, r, i-r, col_i+1);
		r = i;
	}
}

void unique_rows(vector<vector<int> > &matrix) {
	sort_matrix(matrix, 0, matrix.size(), 0);
}

int main(void) {
	cout << "Enter matrix dimensions, followed by the matrix elements" << endl;
	cout << "> ";

	vector<vector<int> >::size_type rows;
	vector<int>::size_type cols;

	while (cin >> rows >> cols) {
		vector<vector<int> > matrix(rows, vector<int>(cols));
		for (vector<vector<int> >::size_type i = 0; i < rows; i++)
			for (vector<int>::size_type j = 0; j < cols; j++)
				cin >> matrix[i][j];

		cout << "Unique rows:" << endl;
		unique_rows(matrix);
		cout << "> ";

	}

	return 0;
}
