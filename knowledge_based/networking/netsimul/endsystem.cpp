
#include <iostream>
#include <cstring>
#include <cstdlib>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

static unsigned char frame_buff[1526];

static uint64_t mac_address;

void send_frame(int linkfd, uint64_t dest_mac, size_t data_len) {

	size_t i;
	for (i = 0; i < 7; i++) {
		frame_buff[i] = 0xAA;
	}

	frame_buff[i++] = 0xAB;

	uint64_t mac_mask = 0xFF0000000000;
	for (uint64_t shift = 5, mask = mac_mask; mask != 0; shift--, mask >>= 8U) {
		frame_buff[i++] = (mac_address & mask) >> shift*8;
	}
	for (uint64_t shift = 5, mask = mac_mask; mask != 0; shift--, mask >>= 8U) {
		frame_buff[i++] = (dest_mac & mask) >> shift*8;
	}

	// TODO Do not hardcode type
	frame_buff[i++] = 0;
	frame_buff[i++] = 0;

	cout << "Current frame offset = " << i << endl;
	cout << "Endsystem waiting for " << data_len << " bytes of data" << endl;

	for (size_t j = 0; j < data_len; j++) {
		cin >> hex >> frame_buff[i++];
	}

	// TODO CRCs
	for (size_t j = 0; j < 4; j++) {
		frame_buff[i++] = 0;
	}

	cout << "Endsystem sending frame of size " << i << endl;

	write(linkfd, frame_buff, i);
}

int main(void) {

	cin >> hex >> mac_address;
	string link;
	cin >> link;

	int fd;

	if ((fd = open(link.c_str(), O_WRONLY)) < 0) {
		cerr << "Error connecting to output link: " << strerror(errno) << endl;
		exit(EXIT_FAILURE);
	}

	cout << "*** Initialized" << endl;

	while (1) {
		uint64_t dest;
		cin >> hex >> dest;

		size_t data_len;
		cin >> dec >> data_len;

		send_frame(fd, dest, data_len);
	}

	return 0;
}
