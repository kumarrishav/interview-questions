/* Router
 * Compile with: g++ -Wall -std=c++11 -Iinclude -o router iface.cpp linklayer.cpp router.cpp
 */

#include <iostream>
#include <iomanip>
#include <vector>

#include <cstdint>
#include <cstdio>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "iface.h"
#include "linklayer.h"

using namespace std;

int main(void) {

	vector<IFace> ifaces;

	unsigned ifaces_n;
	cin >> ifaces_n;
	for (unsigned i = 0; i < ifaces_n; i++) {
		// MAC in-addr out-addr
		uint64_t mac;
		string in, out;
		cin >> hex >> mac;
		cin >> in >> out;

		int fd_in, fd_out;

		if ((fd_in = open(in.c_str(), O_RDONLY)) < 0) {
			cerr << "Error connecting to link " << in << ": " << strerror(errno) << endl;
			exit(EXIT_FAILURE);
		}

		cout << "*** Connection to input link " << in << " established" << endl;

		if ((fd_out = open(out.c_str(), O_WRONLY)) < 0) {
			cerr << "Error connecting to link " << out << ": " << strerror(errno) << endl;
			exit(EXIT_FAILURE);
		}

		cout << "*** Connection to output link " << out << " established" << endl;

		ifaces.push_back(IFace(mac, fd_in, fd_out));
	}

	LinkLayer linklayer(ifaces);
	linklayer.enter_main_loop();

	return 0;
}
