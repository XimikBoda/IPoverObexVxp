#include "TCP.h"
#include "IPtoStream.h"
#include <cstring>
#include <console.h>

TCP::TCP(IPtoStream& owner_, uint8_t type) : owner(owner_), my_type(type) {}

void TCP::parsePacket() {
	if (TCPsocks.is_active(owner.id))
		TCPsocks[owner.id].parsePacket();
}

void TCP::updateData() {
	for (int i = 0; i < TCPsocks.size(); ++i)
		if (TCPsocks.is_active(i))
			TCPsocks[i].updateData();
}

void TCP::update() {
	for (int i = 0; i < TCPsocks.size(); ++i)
		if (TCPsocks.is_active(i))
			TCPsocks[i].update();
}

int TCP::init(tcp_callback_t callback)
{
	int id = TCPsocks.init_new_el();
	if (id == -1)
		return -1;

	TCPSock& tcpsock = TCPsocks[id];

	tcpsock.id = id;
	tcpsock.type_id = owner.writer.makeTypeId(my_type, id);
	tcpsock.owner = this;
	tcpsock.status = TCPSock::ConnectPending;
	tcpsock.callback = callback;

	tcpsock.update();

	return id;
}

int TCP::connect(const char* host, uint16_t port, tcp_callback_t callback) {
	int id = TCPsocks.init_new_el();
	if (id == -1)
		return -1;

	TCPSock& tcpsock = TCPsocks[id];

	strcpy(tcpsock.host, host);
	tcpsock.port = port;
	tcpsock.id = id;
	tcpsock.type_id = owner.writer.makeTypeId(my_type, id);
	tcpsock.owner = this;
	tcpsock.status = TCPSock::ConnectPending;
	tcpsock.callback = callback;

	tcpsock.update();

	return id;
}

ssize_t TCP::write(int id, const void* buf, size_t size) {
	if (TCPsocks.is_active(id))
		return TCPsocks[id].write(buf, size);

	return -1; //todo
}

ssize_t TCP::read(int id, void* buf, size_t size) {
	if (TCPsocks.is_active(id))
		return TCPsocks[id].read(buf, size);

	return -1; //todo
}

void TCP::close(int id) {
	if (TCPsocks.is_active(id))
		return TCPsocks[id].close();
}
