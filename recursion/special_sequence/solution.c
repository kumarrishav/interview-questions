

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
