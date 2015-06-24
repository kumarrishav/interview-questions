/* A special sequence is a sequence where no two adjacent numbers are the same,
 * and it only uses the numbers 1, 2, 3 and 4.
 *
 * We are interested in finding every special sequence with a given number of 1s, 2s,
 * 3s and 4s. Let n1 be the desired number of 1s, n2 the desired number of 2s, etc.
 *
 * Write a method that, given n1, n2, n3 and n4, returns how many special sequences exist.
 *
 * EXAMPLE
 * If n1 = n2 = n3 = n4, we are looking for special sequences that have a 1, a 2 a 3 and a 4.
 * There are 24 of these:
 * 1, 2, 3, 4
 * 1, 2, 4, 3
 * 1, 4, 2, 3
 * ...
 *
 * Source: Careercup (Google interview)
 */
#include <stdio.h>

unsigned generate_sequences_aux(unsigned available[4], unsigned still_usable, unsigned last) {
	if (still_usable == 0) {
		return 1;
	}

	unsigned res = 0;
	size_t i;
	for (i = 0; i < 4; i++) {
		if (available[i] == 0 || i+1 == last) {
			continue;
		}
		available[i]--;
		still_usable--;
		res += generate_sequences_aux(available, still_usable, i+1);
		available[i]++;
		still_usable++;
	}

	return res;
}

unsigned generate_sequences(unsigned available[4]) {
	unsigned total = 0;

	size_t i;
	for (i = 0; i < 4; i++) {
		total += available[i];
	}

	if (total == 0) {
		return 0;
	}

	unsigned res = 0;
	for (i = 0; i < 4; i++) {
		if (available[i] > 0) {
			available[i]--;
			res += generate_sequences_aux(available, total-1, i+1);
			available[i]++;
		}
	}

	return res;
}

int main(void) {

	unsigned values[4];

	printf("Enter n1, n2, n3 and n4\n");
	printf("> ");

	while (scanf("%u%u%u%u", &values[0], &values[1], &values[2], &values[3]) == 4) {
		printf("There are %u possible sequences\n", generate_sequences(values));
		printf("> ");
	}

	return 0;
}
