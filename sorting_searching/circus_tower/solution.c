/* A circus is designing a tower routine consisting of people standing atop one
 * another's shoulders. For practical and aesthetic reasons, each person must be
 * both shorter and lighter than the person below him or her. Given the heights
 * and weights of each person in the circus, write a method to compute the largest
 * possible number of people in such a tower.
 * EXAMPLE:
 * Input (ht, wt): (65, 100) (70, 150) (56, 90) (75, 190) (60, 95)
 *                 (68, 110)
 * Output: The longest tower is length 6 and includes from top to bottom:
 *         (56, 90) (60, 95) (65, 100) (68, 110) (70, 150) (75, 190)
 *
 * Source: Cracking the Coding Interview, page 122, question 11.7
 */
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
