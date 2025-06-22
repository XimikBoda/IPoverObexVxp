#include "TCP.h"
#include "IPtoStream.h"
#include <cstring>

TCP::TCP(IPtoStream& owner_, uint8_t type) : owner(owner_), my_type(type) {}

bool TCP::make_connect_packet(int id) {
	uint16_t type_id = owner.makeTypeId(my_type, id);
	auto& writer = owner.writer;

	TCP_sock& tcpsock = TCPsocks[id];

	writer.init(type_id);
	writer.putString(tcpsock.host);
	writer.putUInt16(tcpsock.port);
	//writer.

	return true;
}

int TCP::connect(const char* host, uint16_t port, std::function<void(int handle, int event)> callback) {
	int id = TCPsocks.get_fist_free();
	if (id == -1)
		return -1;

	TCP_sock& tcpsock = TCPsocks[id];

	strcpy(tcpsock.host, host);
	tcpsock.port = port;

	make_connect_packet(id);

	return id;
}

