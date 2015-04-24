/* Given a binary tree, design an algorithm which creates a linked list of all the
 * nodes at each depth (e.g. if you have a tree with depth D, you'll have D linked
 * lists).
 *
 * Source: Cracking the Coding Interview, page 86, question 4.4
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

struct tree_node {
	int data;
	struct tree_node *left;
	struct tree_node *right;
};

struct list_node {
	void *data;
	struct list_node *prev;
	struct list_node *next;
};

/* Core linked list API */
void list_init_head(struct list_node *n) {
	n->prev = n->next = n;
}

void list_set_data(struct list_node *n, void *data) {
	n->data = data;
}

void list_insert_after(struct list_node *after_this, struct list_node *new_node) {
	new_node->next = after_this->next;
	new_node->prev = after_this;

	after_this->next->prev = new_node;
	after_this->next = new_node;
}

void list_append(struct list_node *head, struct list_node *node) {
	list_insert_after(head->prev, node);
}

void list_delete(struct list_node *node) {
	node->prev->next = node->next;
	node->next->prev = node->prev;

	// Not needed, but it's safer
	node->prev = node->next = node;
}

struct list_node *list_peek(struct list_node *head) {
	return head->next;
}

int list_is_empty(struct list_node *head) {
	return head->next == head;
}

/* A queue of `tree_node *` implemented on top of the linked list */
typedef struct list_node *Queue;

Queue new_queue(void) {
	Queue ret = malloc(sizeof(*ret));

	if (ret == NULL)
		return NULL;

	list_init_head(ret);

	return ret;
}

int enqueue(Queue queue, struct tree_node *node) {
	struct list_node *new_node = malloc(sizeof(*new_node));

	if (new_node == NULL) {
		return 0;
	}

	list_set_data(new_node, node);
	list_append(queue, new_node);

	return 1;
}

struct tree_node *dequeue(Queue queue) {
	assert(!list_is_empty(queue));

	struct list_node *node = list_peek(queue);
	list_delete(node);

	struct tree_node *ret = node->data;
	free(node);

	return ret;
}

void destroy_queue(Queue queue) {
	// TODO
}

int queue_is_empty(Queue q) {
	return list_is_empty(q);
}

/* The solution */
#define LIST_BUFF_SZ 2048
static struct list_node list_nodes[LIST_BUFF_SZ];
static size_t next_node;

static size_t enqueue_all_children(Queue queue, struct tree_node *node) {
	size_t res = 0;

	if (node->left != NULL) {
		enqueue(queue, node->left);
		res++;
	}

	if (node->right != NULL) {
		enqueue(queue, node->right);
		res++;
	}

	return res;
}

size_t make_lists(struct list_node out[], struct tree_node *root) {

	if (root == NULL) {
		return 0;
	}

	Queue queue = new_queue();
	if (queue == NULL) {
		return 0;
	}

	unsigned curr_level = 1;
	unsigned next_level = 0;
	size_t cursor = 0;
	enqueue(queue, root);

	while (!queue_is_empty(queue)) {
		list_init_head(&out[cursor]);

		size_t i;
		for (i = 0; i < curr_level; i++) {
			struct tree_node *node = dequeue(queue);
			next_level += enqueue_all_children(queue, node);

			struct list_node *list_el = &list_nodes[next_node++];
			list_set_data(list_el, node);
			list_append(&out[cursor], list_el);
		}

		cursor++;
		curr_level = next_level;
		next_level = 0;
	}

	return cursor;
}

#define MAX_NODES 1024
static struct tree_node tree_nodes[MAX_NODES];
static size_t nodes_in_use;

static struct list_node lists_array[MAX_NODES];

int main(void) {

	printf("Input format:\n");
	printf("<number of nodes>\n");
	printf("<number of children of node0> <child1> <child2>\n");
	printf("<number of children of node1> <child1> <child2>\n");
	printf("node IDs go from 0 to <number of nodes>-1\n");
	printf("The root is node 0\n");

	scanf("%zu", &nodes_in_use);

	size_t i;
	for (i = 0; i < nodes_in_use; i++) {
		tree_nodes[i].data = i;

		size_t children;
		scanf("%zu", &children);

		size_t child;
		if (children > 2)
			children = 2;

		if (children > 0) {
			scanf("%zu", &child);
			tree_nodes[i].left = &tree_nodes[child];
		}

		if (children > 1) {
			scanf("%zu", &child);
			tree_nodes[i].right = &tree_nodes[child];
		}

	}

	size_t lists_sz = make_lists(lists_array, &tree_nodes[0]);
	printf("Layer by layer list:\n");

	for (i = 0; i < lists_sz; i++) {
		struct list_node *head = &lists_array[i];
		struct list_node *cursor;
		for (cursor = head->next; cursor != head; cursor = cursor->next) {
			printf("%d ", ((struct tree_node *) cursor->data)->data);
		}
		printf("\n");
	}
	

	return 0;
}
