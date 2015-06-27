
#include "linklayer.h"

#include <iostream>

#include <cstring>
#include <cerrno>
#include <cstdint>
#include <unistd.h>

/* The Ethernet frame is composed by the following fields:
 *
 * Preamble [8 bytes]: 7 bytes with the binary value "10101010" followed by a byte with the value
 * "10101011". The first 7 bytes serve to wake up the receiving adapters and to synchronize their
 * clocks to that of the sender's. This is not really needed in this simulated environment,
 * but we keep it for completeness.
 *
 * Source address [6 bytes]: Ethernet uses 6-bytes link layer addresses (also known as
 * MAC Addresses).
 *
 * Destination address [6 bytes]: The destination MAC address.
 *
 * Type [2 bytes]: Used to multiplex the frame to the next layer protocol. Typically this field
 * encodes the type for the IP protocol, but another common case is when the frame is an ARP
 * message (type 0x0806).
 *
 * Data [46 - 1500 bytes]: The payload. Typically an IP datagram.
 *
 * CRC [4 bytes]: Cyclic Redundancy Check. Used for validation purposes. Allows error detection
 * at the link layer level.
 */

#define ETHERNET_FRAME_MAX_LEN 1526
#define ETHERNET_FRAME_MIN_LEN 72
#define ETHERNET_FRAME_MAX_DATA_LEN 1500

#define LINKLAYER_LOG_PREFIX "[LinkLayer]: "

static unsigned char frame_buffer[ETHERNET_FRAME_MAX_LEN];

static struct {
	uint64_t src_mac;
	uint64_t dst_mac;
	uint16_t type;
	unsigned char data[ETHERNET_FRAME_MAX_DATA_LEN];
	size_t data_len;
	uint32_t crc;
} eth_frame;

using namespace std;

static bool parse_frame(ssize_t frame_sz) {

	cout << LINKLAYER_LOG_PREFIX "Parsing frame of size " << frame_sz << endl;

	for (int i = 0; i < 7; i++) {
		if (frame_buffer[i] != 0xAA) {
			cerr << LINKLAYER_LOG_PREFIX "Wrong preamble value at byte offset " << i << endl;
			return false;
		}
	}

	if (frame_buffer[7] != 0xAB) {
		cerr << LINKLAYER_LOG_PREFIX "Wrong preamble value at byte offset 7" << endl;
		return false;
	}

	eth_frame.src_mac = 0;
	for (int i = 0; i < 6; i++) {
		eth_frame.src_mac = (eth_frame.src_mac << 8) | frame_buffer[i+8];
	}

	eth_frame.dst_mac = 0;
	for (int i = 0; i < 6; i++) {
		eth_frame.dst_mac = (eth_frame.dst_mac << 8) | frame_buffer[i+14];
	}

	eth_frame.type = (frame_buffer[20] << 8) | frame_buffer[21];

	for (int i = 22; i < frame_sz-4; i++) {
		eth_frame.data[i-22] = frame_buffer[i];
	}

	eth_frame.data_len = frame_sz-26;

	// CRC lives in frame_buffer[frame_sz-4..frame_sz-1]
	// TODO Validate CRC

	return true;
}

// The core
void LinkLayer::enter_main_loop(void) {
	while (1) {

		cout << LINKLAYER_LOG_PREFIX "In main loop" << endl;

		int ready = poll(&pollfds[0], pollfds.size(), -1);
		if (ready < 0) {
			cerr << "Poll error: " << strerror(errno) << endl;
			continue;
		}

		cout << LINKLAYER_LOG_PREFIX "poll(2) returned: " << ready << endl;

		for (vector<pollfd>::const_iterator it = pollfds.begin(); it != pollfds.end(); it++) {
			if (it->revents & POLLIN) {
				ssize_t n;
				if ((n = read(it->fd, frame_buffer, sizeof(frame_buffer))) < 0) {
					cerr << "read(2) error: " << strerror(errno) << endl;
					continue;
				}

				if (n < ETHERNET_FRAME_MIN_LEN) {
					cerr << LINKLAYER_LOG_PREFIX "Invalid frame of size " << n << " received, ignoring..." << endl;
					continue;
				}

				if (!parse_frame(n)) {
					continue;
				}

				const IFace *iface = find_iface_in(it->fd);

				if (eth_frame.dst_mac != iface->get_mac_addr()) {
					cerr << LINKLAYER_LOG_PREFIX "Got frame with destination MAC " << hex << eth_frame.dst_mac
					     << " on interface " << hex << iface->get_mac_addr() << ", ignoring..." << endl;
					continue;
				}

				cout << LINKLAYER_LOG_PREFIX "New frame arrived on interface " << hex << iface->get_mac_addr() << endl;
				cout << LINKLAYER_LOG_PREFIX "Source address: " << hex << eth_frame.src_mac << endl;
				cout << LINKLAYER_LOG_PREFIX "Destination address: " << hex << eth_frame.dst_mac << endl;
				cout << LINKLAYER_LOG_PREFIX "Type: " << hex << eth_frame.type << endl;
				cout << LINKLAYER_LOG_PREFIX "Data:";
				for (size_t i = 0; i < eth_frame.data_len; i++) {
					cout << " " << hex << eth_frame.data[i];
				}
				cout << endl;
				cout << LINKLAYER_LOG_PREFIX "CRC: " << hex << eth_frame.crc << endl;

				// TODO Deliver to network layer
			}
		}
	}
}
