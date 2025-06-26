#include "TCP.h"
#include "IPtoStream.h"
#include <cstring>

TCP::TCP(IPtoStream& owner_, uint8_t type) : owner(owner_), my_type(type) {}

void TCP_sock::sendCallbackEvent(TCPEvent event) {
	if (callback)
		callback(id, event);
}

bool TCP_sock::make_connect_packet() {
	auto& writer = owner->owner.writer;
	uint16_t type_id = writer.makeTypeId(owner->my_type, id);

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

void TCP::parsePacket() {
	if (TCPsocks.is_active(owner.id))
		TCPsocks[owner.id].parsePacket();
}

int TCP::connect(const char* host, uint16_t port, tcp_callback_t callback) {
	int id = TCPsocks.init_new_el();
	if (id == -1)
		return -1;

	TCP_sock& tcpsock = TCPsocks[id];

	strcpy(tcpsock.host, host);
	tcpsock.port = port;
	tcpsock.id = id;
	tcpsock.owner = this;
	tcpsock.status = TCP_sock::ConnectPending;
	tcpsock.callback = callback;

	tcpsock.update();

	return id;
}

