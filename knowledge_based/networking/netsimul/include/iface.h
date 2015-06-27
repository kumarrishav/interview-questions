#ifndef IFACE_H
#define IFACE_H

#include <cstdint>
#include <sstream>
#include <algorithm>

#include <iostream>

class IFace {
	uint64_t mac_address;
	std::string mac_addr_str;

	int fd_in;
	int fd_out;

public:
	IFace(uint64_t addr, int in, int out) : mac_address(addr), fd_in(in), fd_out(out) {
		std::ostringstream oss;
		oss << std::hex << (addr & (uint64_t) 0xFF);
		addr >>= 8;
		while (addr > 0) {
			oss << ":" << std::hex << (addr & (uint64_t) 0xFF);
			addr >>= 8;
		}
		mac_addr_str = oss.str();
		std::reverse(mac_addr_str.begin(), mac_addr_str.end());
	}

	int get_fd_in(void) const { return fd_in; }
	uint64_t get_mac_addr(void) const { return mac_address; }
};

#endif /* IFACE_H */
