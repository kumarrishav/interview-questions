/* A toy implementation of a hash table */
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

/* This is a prime number not too close to a power of 2. Usually works good */
#define HASH_TABLE_SZ 941

typedef int Key;
typedef int Item;

struct ht_node {
	Item value;
	Key key;
	struct ht_node *next;
	struct ht_node *prev;
};

struct ht_node *new_node(Item v, Key k) {
	struct ht_node *res = malloc(sizeof(*res));

	if (res == NULL)
		return NULL;

	res->value = v;
	res->key = k;
	res->next = res->prev = res;

	return res;
}

static void free_node(struct ht_node *node) {
	free(node);
}

static void insert_after(struct ht_node *new, struct ht_node *head) {
	new->next = head->next;
	new->prev = head;
	head->next->prev = new;
	head->next = new;
}

static void delete_node(struct ht_node *node) {
	node->prev->next = node->next;
	node->next->prev = node->prev;
	free_node(node);
}

static void destroy_list(struct ht_node *head) {
	struct ht_node *cursor = head->next;
	struct ht_node *next_cursor;

	while (cursor != head) {
		next_cursor = cursor->next;
		free(next_cursor);
		cursor = next_cursor;
	}

	head->next = head->prev = head;
}

struct hash_table {
	struct ht_node hash_table[HASH_TABLE_SZ];
};

// Just a simple hash function, could be more complex in the real world
static size_t hash_key(Key k) {
	// Assumes there's a well behaved conversion of `Key` to `unsigned`
	return ((unsigned) k) % 941;
}

struct hash_table *new_table(void) {
	struct hash_table *res = malloc(sizeof(*res));

	if (res == NULL)
		return NULL;

	size_t i;
	for (i = 0; i < HASH_TABLE_SZ; i++)
		res->hash_table[i].next = res->hash_table[i].prev = &res->hash_table[i];

	return res;
}

void free_table(struct hash_table *table) {
	size_t slot;
	for (slot = 0; slot < HASH_TABLE_SZ; slot++) {
		destroy_list(&table->hash_table[slot]);
	}
}

/* Assumes element is not on the table yet. */
int insert(struct hash_table *table, Key k, Item value) {
	assert(table != NULL);

	size_t slot = hash_key(k);
	assert(slot < HASH_TABLE_SZ);

	struct ht_node *node = new_node(value, k);
	if (node == NULL)
		return 0;

	insert_after(node, &table->hash_table[slot]);
	return 1;
}

static struct ht_node *find_internal(struct ht_node *head, Key k) {
	struct ht_node *cursor;
	for (cursor = head->next; cursor != head && cursor->key != k; cursor = cursor->next)
		; /* Intentionally left blank */
	return cursor;
}

int find(struct hash_table *table, Key k, Item *out) {
	assert(table != NULL);

	size_t slot = hash_key(k);
	assert(slot < HASH_TABLE_SZ);

	struct ht_node *head = &table->hash_table[slot];
	struct ht_node *element = find_internal(head, k);

	if (element == head)
		return 0;

	*out = element->value;
	return 1;
}

void delete(struct hash_table *table, Key k) {
	assert(table != NULL);

	size_t slot = hash_key(k);
	assert(slot < HASH_TABLE_SZ);

	struct ht_node *head = &table->hash_table[slot];
	struct ht_node *element = find_internal(&table->hash_table[slot], k);

	if (element != head) {
		delete_node(element);
	}
}

static char input[256];
int main(void) {
	printf("I K V - Inserts the key K with value V in the hash table\n");
	printf("D K - Deletes the key K from the hash table\n");
	printf("S K - Searches the hash table for this key and prints the result\n");

	struct hash_table *htable = new_table();
	if (htable == NULL) {
		fprintf(stderr, "Cannot create hash table: out of memory\n");
		return 1;
	}

	Key k;
	Item v;

	while (fgets(input, sizeof(input), stdin) != NULL) {
		if (sscanf(input, "I %d %d", &k, &v) == 2) {
			if (!insert(htable, k, v)) {
				fprintf(stderr, "Couldn't insert new element - out of memory.\n");
				exit(0);
			}
		} else if (sscanf(input, "D %d", &k) == 1) {
			delete(htable, k);			
		} else if (sscanf(input, "S %d", &k) == 1) {
			if (find(htable, k, &v)) {
				printf("%d: %d\n", k, v);
			} else {
				printf("No item with key %d\n", k);
			}
		} else {
			fprintf(stderr, "Invalid input.\n");
			assert(0);
		}
	}

	return 0;
}
