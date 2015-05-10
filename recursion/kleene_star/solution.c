
#include <stdio.h>
#include <assert.h>
#define BUFF_SZ 512

/* ~~~ Question (a) ~~~ */
struct node {
	struct node *next[2];
	unsigned char accept;
};

struct node *step(unsigned input, struct node *curr) {
	assert(input < sizeof(curr->next)/sizeof(*curr->next));
	return curr->next[input];
}

int dfa(const char *input, struct node *start) {
	size_t input_i = 0;
	struct node *curr_state = start;

	while (input[input_i] != '\0') {
		curr_state = step(input[input_i]-'0', curr_state);
		input_i++;
	}

	return curr_state->accept;
}

/* ~~~ The DFA of question (a) ~~~ */
static struct node simple_dfa[] = {
	// A
	{ .next = { &simple_dfa[4], &simple_dfa[1] }, .accept = 0 },
	// B
	{ .next = { &simple_dfa[2], &simple_dfa[1] }, .accept = 0 },
	// C
	{ .next = { &simple_dfa[3], &simple_dfa[4] }, .accept = 0 },
	// D
	{ .next = { &simple_dfa[3], &simple_dfa[3] }, .accept = 1 },
	// E
	{ .next = { &simple_dfa[4], &simple_dfa[4] }, .accept = 0 }
};
static struct node *dfa_start_state = &simple_dfa[0];

/* ~~~ Question (b) ~~~ */
#define NFA_MAX_BRANCHING 10

struct nfa_node {
	struct nfa_node *next[2][NFA_MAX_BRANCHING+1]; // Null terminated
	struct nfa_node *epsilon[NFA_MAX_BRANCHING+1]; // Null terminated
	unsigned char accept;
	unsigned char visited;
	char id;
};

/* ~~~ The NFA for question (b) ~~~ */
static struct nfa_node simple_nfa[] = {
	// A
	{ .next = { { NULL }, { NULL } },
	  .epsilon = { &simple_nfa[1], &simple_nfa[2], NULL },
	  .accept = 0,
	  .visited = 0,
	  .id = 'A'
	},
	// B
	{ .next = { { NULL }, { &simple_nfa[3], NULL } },
	  .epsilon = { NULL },
	  .accept = 0,
	  .visited = 0,
	  .id = 'B'
	},
	// C
	{ .next = { { &simple_nfa[4], NULL }, { NULL } },
	  .epsilon = { NULL },
	  .accept = 0,
	  .visited = 0,
	  .id = 'C'
	},
	// D
	{ .next = { { NULL }, { &simple_nfa[5], NULL } },
	  .epsilon = { NULL },
	  .accept = 0,
	  .visited = 0,
	  .id = 'D'
	},
	// E
	{ .next = { { &simple_nfa[6], NULL }, { NULL } },
	  .epsilon = { NULL },
	  .accept = 0,
	  .visited = 0,
	  .id = 'E'
	},
	// F
	{ .next = { { NULL }, { NULL } },
	  .epsilon = { &simple_nfa[0], &simple_nfa[7], NULL },
	  .accept = 0,
	  .visited = 0,
	  .id = 'F'
	},
	// G
	{ .next = { { NULL }, { NULL } },
	  .epsilon = { &simple_nfa[0], &simple_nfa[7], NULL },
	  .accept = 0,
	  .visited = 0,
	  .id = 'G'
	},
	// H
	{ .next = { { NULL }, { NULL } },
	  .epsilon = { NULL },
	  .accept = 1,
	  .visited = 0,
	  .id = 'H'
	}
};

static void nfa_prepare_visit(void) {
	size_t i;
	size_t lim = sizeof(simple_nfa)/sizeof(simple_nfa[0]);
	for (i = 0; i < lim; i++) {
		simple_nfa[i].visited = 0;
	}
}

static size_t kleene_closure_aux(char *buff, size_t buff_i, struct nfa_node *curr_node) {
	buff_i += sprintf(&buff[buff_i], "%c ", curr_node->id);
	curr_node->visited = 1;

	size_t i;
	for (i = 0; curr_node->epsilon[i] != NULL; i++) {
		if (!curr_node->epsilon[i]->visited) {
			buff_i = kleene_closure_aux(buff, buff_i, curr_node->epsilon[i]);
		}
	}

	return buff_i;
}

void kleene_closure(struct nfa_node *node, char *out) {
	nfa_prepare_visit();
	kleene_closure_aux(out, 0, node);
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

static char input_buff[BUFF_SZ];
int main(void) {
	printf("Available commands are:\n"
	       "A <input> - Run the DFA of question (a) with a given input. The input must be a string with no spaces and only 0s and 1s\n"
	       "K <node> - Compute the kleene closure of node <node>. Can be any node in the range [a-h]\n"
	       "Q - Quit\n"
	       "> ");

	char op;
	while (scanf(" %c", &op) == 1) {
		if (op == 'A') {
			scanf("%s", input_buff);
			printf("Input string: %s\n", input_buff);
			if (dfa(input_buff, dfa_start_state)) {
				printf("Accepted\n");
			} else {
				printf("Not accepted\n");
			}
		} else if (op == 'K') {
			char node;
			scanf(" %c", &node);
			assert(node >= 'a' && node <= 'h');
			kleene_closure(&simple_nfa[(unsigned char) (node-'a')], input_buff);
			printf("Kleene closure for node %c: %s\n", node, input_buff);
		} else if (op == 'Q') {
			break;
		} else {
			fprintf(stderr, "Unknown command: %c\n", op);
		}
		printf("> ");
	}

	return 0;
}
