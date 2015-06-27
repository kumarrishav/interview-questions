#ifndef LINK_LAYER_H
#define LINK_LAYER_H

#include "iface.h"

#include <cassert>
#include <poll.h>

#include <vector>

class LinkLayer {
	std::vector<IFace> ifaces;
	std::vector<pollfd> pollfds;

	const IFace *find_iface_in(int fd_in) const {
		std::vector<IFace>::size_type i;
		for (i = 0; i < ifaces.size() && ifaces[i].get_fd_in() != fd_in; i++);
		assert(i < ifaces.size());
		return &ifaces[i];

	}

public:
	LinkLayer(const std::vector<IFace> &ifs) : ifaces(ifs) {
		for (std::vector<IFace>::const_iterator it = ifaces.begin(); it != ifaces.end(); it++) {
			pollfd pfd;
			pfd.fd = it->get_fd_in();
			pfd.events = POLLIN;
			pollfds.push_back(pfd);
		}
	}

	void enter_main_loop(void);
};

#endif /* LINK_LAYER_H */
