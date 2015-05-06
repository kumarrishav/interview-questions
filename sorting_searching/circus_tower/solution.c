
#include <stdio.h>

struct person {
	unsigned height;
	unsigned weight;
};

static int can_be_above(struct person *below, struct person *above) {
	return above->height < below->height && above->weight < below->weight;
}

static void swap(size_t N, struct person arr[N], size_t i, size_t j) {
	struct person tmp = arr[i];
	arr[i] = arr[j];
	arr[j] = tmp;
}

static unsigned best_tower_aux(size_t N, struct person people[N], size_t person_i) {
	unsigned res = 0;
	size_t i;
	for (i = person_i; i < N; i++) {
		if (person_i > 0 && !can_be_above(&people[person_i-1], &people[i])) {
			continue;
		}

		swap(N, people, person_i, i);
		unsigned best = best_tower_aux(N, people, person_i+1)+1;
		if (best > res) {
			res = best;
		}
		swap(N, people, person_i, i);
	}

	return res;
}

unsigned best_tower(size_t N, struct person people[N]) {
	return best_tower_aux(N, people, 0);
}

int main(void) {
	printf("Enter the number of people, followed by each person's height and weight\n");

	size_t people_sz;
	while (scanf("%zu", &people_sz) == 1) {
		struct person people[people_sz];

		size_t i;
		for (i = 0; i < people_sz; i++) {
			scanf("%u%u", &people[i].height, &people[i].weight);
		}

		printf("Max. people to form a tower: %u\n", best_tower(people_sz, people));
	}

	return 0;
}
