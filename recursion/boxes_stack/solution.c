/* You have a stack of n boxes, with widths wi, heights hi, and depths di. The boxes
 * cannot be rotated and can only be stacked on top of one another if each box
 * in the stack is strictly larger than the box above it in width, height and depth.
 * Implement a method to build the tallest stack possible, where the height of a 
 * stack is the sum of the heights of each box.
 *
 * Source: Cracking the Coding Interview, page 110, question 9.10
 */
#include <stdio.h>

/* Note: We only return the height of the tallest stack possible.
 * Returning the actual elements would involve memory allocation and a lot
 * of copying. This would be better implemented in languages with better library
 * support like C++
 */

struct box {
	unsigned width;
	unsigned height;
	unsigned depth;
};

static int can_stack(size_t boxes_no, struct box boxes[boxes_no],
		     size_t box_below, size_t box_above) {
	return boxes[box_above].width < boxes[box_below].width &&
		boxes[box_above].height < boxes[box_below].height &&
		boxes[box_above].depth < boxes[box_below].depth;
}

static void swap(size_t n, struct box arr[n], size_t x, size_t y) {
	struct box tmp = arr[x];
	arr[x] = arr[y];
	arr[y] = tmp;
}

static unsigned stack_boxes_aux(size_t boxes_no, struct box boxes[boxes_no], size_t box_i,
				unsigned curr_height) {

	unsigned res = curr_height;
	size_t box;
	for (box = box_i; box < boxes_no; box++) {
		if (box_i > 0 && !can_stack(boxes_no, boxes, box_i-1, box)) {
			continue;
		}

		swap(boxes_no, boxes, box_i, box);
		size_t h = stack_boxes_aux(boxes_no, boxes, box_i+1,
					   curr_height+boxes[box_i].height);
		if (h > res) {
			res = h;
		}
		swap(boxes_no, boxes, box_i, box);
	}

	return res;
}

unsigned tallest_stack(size_t boxes_no, struct box boxes[boxes_no]) {
	return stack_boxes_aux(boxes_no, boxes, 0, 0);
}

int main(void) {
	printf("Enter number of boxes, and then for each box enter width, height and depth.\n");

	size_t n;
	while (scanf("%zu", &n) == 1) {
		struct box boxes[n];
		size_t i;
		for (i = 0; i < n; i++) {
			scanf("%u%u%u", &boxes[i].width, &boxes[i].height, &boxes[i].depth);
		}
		printf("Height: %u\n", tallest_stack(n, boxes));
	}

	return 0;
}
