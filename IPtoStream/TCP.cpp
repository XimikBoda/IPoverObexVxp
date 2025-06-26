#include "TCP.h"
#include "IPtoStream.h"
#include <cstring>

TCP::TCP(IPtoStream& owner_, uint8_t type) : owner(owner_), my_type(type) {}

void TCP_sock::sendCallbackEvent(TCPEvent event) {
	if (callback)
		callback(id, event);
}

void TCP_sock::send() {
	if (!send_buf_pos)
		return;

	auto& writer = owner->owner.writer;
	if (!writer.available())
		return;

	writer.init(type_id);
	writer.putUInt8(TCP::Send);

	size_t size = send_buf_pos;
	size_t free_size = writer.free_size();
	if (size > free_size)
		size = free_size;

	writer.putBuf(send_buf, size);
	writer.send();

	if(send_buf_pos - size)
		memmove(send_buf, send_buf + size, send_buf_pos - size);

	send_buf_pos -= size;
}


bool TCP_sock::make_connect_packet() {
	auto& writer = owner->owner.writer;

	if (!writer.available())
		return false;

	writer.init(type_id);
	writer.putUInt8(TCP::Connect);
	writer.putString(host);
	writer.putUInt16(port);
	writer.send();

	return true;
}

void TCP_sock::parseTCPConnectPacket() {
	TCP::RspStatus rstatus = (TCP::RspStatus)owner->owner.reader.readUInt8();

	switch (rstatus)
	{
	case TCP::Done:
		if (status == TCPStatus::ConnectSent) {
			status = TCPStatus::Connected;
			sendCallbackEvent(TCPEvent::Connected);
		}
		else
			status = TCPStatus::Error;
		break;
	case TCP::NotReady:
	case TCP::Disconnected:
	case TCP::Error:
	case TCP::Busy:
	default:
		status = TCPStatus::Error;
		sendCallbackEvent(TCPEvent::Error);
		break;
	}
}

void TCP_sock::parsePacket() {
	uint8_t act = owner->owner.reader.readUInt8();

	switch (act) {
	case TCP::Connect:
		parseTCPConnectPacket();
		break;
	}
}

void TCP_sock::update() {
	switch (status)
	{
	case TCP_sock::None:
		break;
	case TCP_sock::ConnectPending:
		if (make_connect_packet())
			status = TCP_sock::ConnectSent;
		break;
	case TCP_sock::ConnectSent:
		break;
	case TCP_sock::Connected:
		break;
	default:
		break;
	}
}

void TCP_sock::updateData() {
	if (status == TCP_sock::Connected)
		send();
}

ssize_t TCP_sock::write(const void* buf, size_t size) {
	int free_size = tcp_send_buf_size - send_buf_pos;
	if (size > free_size)
		size = free_size;

	if (status == TCPStatus::Error)
		return -2;

	if (size == 0)
		return 0;

	memcpy(send_buf + send_buf_pos, buf, size);
	send_buf_pos += size;

	updateData();

	return size;
}

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

int TCP::connect(const char* host, uint16_t port, tcp_callback_t callback) {
	int id = TCPsocks.init_new_el();
	if (id == -1)
		return -1;

	TCP_sock& tcpsock = TCPsocks[id];

	strcpy(tcpsock.host, host);
	tcpsock.port = port;
	tcpsock.id = id;
	tcpsock.id = owner.writer.makeTypeId(my_type, id);
	tcpsock.owner = this;
	tcpsock.status = TCP_sock::ConnectPending;
	tcpsock.callback = callback;

	tcpsock.update();

	return id;
}

ssize_t TCP::write(int id, const void* buf, size_t size) {
	if (TCPsocks.is_active(id))
		return TCPsocks[id].write(buf, size);

	return -1; //todo
}