/* You are given a set of airline tickets with the source and destination airports.
 * Write a method that takes a set of tickets and reconstructs the itinerary.
 *
 * EXAMPLE
 *
 * Input:
 * [MUC, LHR], [CDG, MUC], [SFO, SJC], [LHR, SFO]
 *
 * Output:
 * [CDG, MUC] -> [MUC, LHR] -> [LHR, SFO] -> [SFO, SJC]
 *
 * Source: Careercup (Google interview)
 */
#include <iostream>
#include <set>
#include <map>
#include <vector>

using namespace std;

struct ticket {
	string source;
	string destination;
};

vector<ticket> get_itinerary(const vector<ticket> &tickets) {
	set<string> destinations;
	map<string, ticket> sources;

	for (vector<ticket>::const_iterator ticket_it = tickets.begin();
	     ticket_it != tickets.end();
	     ticket_it++) {
		sources[ticket_it->source] = *ticket_it;
		destinations.insert(ticket_it->destination);
	}

	string src;
	for (map<string, ticket>::const_iterator ticket_it = sources.begin();
	     ticket_it != sources.end();
	     ticket_it++) {
		if (destinations.find(ticket_it->first) == destinations.end()) {
			src = ticket_it->first;
			break;
		}
	}

	vector<ticket> res;

	while (sources.find(src) != sources.end()) {
		res.push_back(sources[src]);
		src = sources[src].destination;
	}

	return res;
}

void print_itinerary(const vector<ticket> &itinerary) {
	if (itinerary.size() > 0) {
		cout << "[" << itinerary[0].source << ", " << itinerary[0].destination << "]";
	}

	for (vector<ticket>::size_type i = 1; i < itinerary.size(); i++) {
		cout << " -> [" << itinerary[i].source << ", " << itinerary[i].destination << "]";
	}

	cout << endl;
}

int main(void) {
	cout << "Enter the number of tickets, followed by each ticket's source and destination" << endl;
	cout << "> ";

	vector<string>::size_type tickets_count;
	while (cin >> tickets_count) {

		vector<ticket> tickets;

		for (vector<string>::size_type i = 0; i < tickets_count; i++) {
			string src, dest;
			cin >> src >> dest;

			ticket new_ticket;
			new_ticket.source = src;
			new_ticket.destination = dest;

			tickets.push_back(new_ticket);
		}

		vector<ticket> itinerary = get_itinerary(tickets);

		print_itinerary(itinerary);

		cout << "> ";
	}

	return 0;
}
