/* Routing tables are of paramount importance to keep the internet up and running.
 *
 * How do routing tables get populated? What are the classes of different algorithms that
 * can be used to fill a routing table?
 *
 * Imagine you are to implement a router's routing table. How would you implement it?
 * How can you apply the longest prefix matching rule efficiently?
 * What if we want to save memory?
 *
 * Source: invented (this is kind of an open-ended question)
 *
 * NOTE: Compile with -DTEST_RTABLE to run as a standalone program
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>

struct trie_node {
	unsigned char is_end;
	uint32_t dest;
	struct trie_node *children[2];
};

static struct trie_node root = { 1, ~((uint32_t) 0), { NULL, NULL } };

void set_defl_gateway(uint32_t ip) {
	root.dest = ip;
}

void add_entry(uint32_t ip, uint8_t cidr_val, uint32_t dest) {
	uint32_t curr_mask = 1 << 31;

	uint32_t mask_lim = 0;
	if (cidr_val < 32) {
		mask_lim = 1 << (31 - cidr_val);
	}

	uint32_t curr_val = !!(ip & curr_mask);
	struct trie_node *curr_node = &root;

	while (curr_mask != mask_lim && curr_node->children[curr_val] != NULL) {
		curr_node = curr_node->children[curr_val];
		curr_mask >>= 1;
		curr_val = !!(ip & curr_mask);
	}

	while (curr_mask != mask_lim) {
		curr_node->children[curr_val] = malloc(sizeof(*curr_node->children[curr_val]));
		assert(curr_node->children[curr_val] != NULL);
		curr_node = curr_node->children[curr_val];
		curr_node->is_end = 0;
		curr_node->children[0] = curr_node->children[1] = NULL;
		curr_mask >>= 1;
		curr_val = !!(ip & curr_mask);
	}

	curr_node->is_end = 1;
	curr_node->dest = dest;

}

uint32_t lookup(uint32_t ip) {
	uint32_t curr_mask = 1 << 31;
	uint32_t curr_val = !!(ip & curr_mask);
	struct trie_node *curr_node = &root;
	struct trie_node *last_word = &root;

	while (curr_node->children[curr_val] != NULL) {
		curr_node = curr_node->children[curr_val];
		if (curr_node->is_end) {
			last_word = curr_node;
		}
		curr_mask >>= 1;
		curr_val = !!(ip & curr_mask);
	}

	return last_word->dest;
}

#ifdef TEST_RTABLE

static uint32_t read_ip(void) {
	int blocks[4];
	int res = scanf("%d.%d.%d.%d", &blocks[0], &blocks[1], &blocks[2], &blocks[3]);
	assert(res == 4);
	return (blocks[0] << 24) | (blocks[1] << 16) | (blocks[2] << 8) | blocks[3];
}

static void read_ip_mask(uint32_t ip_mask[2]) {
	ip_mask[0] = read_ip();
	int mask;
	int res = scanf("/%d", &mask);
	assert(res == 1);
	ip_mask[1] = mask;
}

static const char *ip_to_str(uint32_t ip) {
	static char buf[12];
	uint32_t byte = 0xFF;
	sprintf(buf, "%" PRIu32 ".%" PRIu32 ".%" PRIu32 ".%" PRIu32,
		(ip >> 24) & byte, (ip >> 16) & byte, (ip >> 8) & byte, ip & byte);
	return buf;
}

void print_table(struct trie_node *node, int depth) {
	char spaces[depth*4+1];
	int i;
	for (i = 0; i < sizeof(spaces)-1; i++) {
		spaces[i] = ' ';
	}
	spaces[i] = '\0';

	printf("%sis_end = %d", spaces, node->is_end);
	if (node->is_end) {
		printf(" (dest = %s)", ip_to_str(node->dest));
	}
	printf("\n");
	if (node->children[0] != NULL) {
		printf("%s%d:\n", spaces, 0);
		print_table(node->children[0], depth+1);
	}
	if (node->children[1] != NULL) {
		printf("%s%d:\n", spaces, 1);
		print_table(node->children[1], depth+1);
	}
}


int main(void) {
	printf("This is a simulation of a router's forwarding table.\n");
	printf("The commands available are:\n");
	printf("D ip - Set the default gateway to `ip`. This should be the first step before any other query\n");
	printf("I ip/mask destination - Insert a new entry for the CIDR-ized address ip/mask to be forwarded to `destination`\n");
	printf("                        Example: I 192.168.1.0/24 192.168.1.254 will forward 192.168.1.0/24 to the router 192.168.1.254\n");
	printf("L ip - Find the next hop for a packet with the destination `ip`. As usual, longest prefix matching is used.\n");
	printf("P - Print the routing table\n");
	printf("Q - quit\n");
	printf("\n> ");

	char op;
	while (scanf(" %c", &op) == 1) {
		if (op == 'D') {
			set_defl_gateway(read_ip());
		} else if (op == 'I') {
			uint32_t ip_mask[2];
			read_ip_mask(ip_mask);
			add_entry(ip_mask[0], ip_mask[1], read_ip());
		} else if (op == 'L') {
			uint32_t ip = read_ip();
			uint32_t res = lookup(ip);
			printf("Dest = %s", ip_to_str(ip));
			printf(", Next hop = %s\n", ip_to_str(res));
		} else if (op == 'P') {
			print_table(&root, 0);
		} else if (op == 'Q') {
			break;
		} else {
			fprintf(stderr, "Unrecognized command: %c\n", op);
		}
		printf("> ");
	}

	return 0;
}
#endif
