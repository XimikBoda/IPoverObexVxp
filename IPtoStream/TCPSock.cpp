#include "TCPSock.h"
#include "TCP.h"
#include "IPtoStream.h"
#include <cstring>
#include <console.h>

void TCPSock::sendCallbackEvent(TCPEvent event, uint32_t val) {
	if (callback)
		callback(id, event, val);
}

void TCPSock::send() {
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

bool TCPSock::make_init_packet() {
	auto& writer = owner->owner.writer;

	if (!writer.available())
		return false;

	writer.init(type_id);
	writer.putUInt8(TCP::Init);
	writer.putVarInt(tcp_receive_buf_size);
	writer.send();

	return true;
}

bool TCPSock::make_connect_packet() {
	auto& writer = owner->owner.writer;

	if (!writer.available())
		return false;

	writer.init(type_id);
	writer.putUInt8(TCP::Connect);
	writer.putString(host);
	writer.putUInt16(port);
	writer.putVarInt(tcp_receive_buf_size);
	writer.send();

	return true;
}

bool TCPSock::make_receive_packet() {
	auto& writer = owner->owner.writer;

	if (!writer.available())
		return false;

	writer.init(type_id);
	writer.putUInt8(TCP::Receive);
	writer.putVarInt(receive_count);
	writer.send();

	return true;
}

bool TCPSock::make_disconnect_packet() {
	auto& writer = owner->owner.writer;

	if (!writer.available())
		return false;

	writer.init(type_id);
	writer.putUInt8(TCP::Disconnect);
	writer.send();

	return true;
}

void TCPSock::parseTCPConnectPacket() {
	TCP::RspStatus rstatus = (TCP::RspStatus)owner->owner.reader.readUInt8();
	ip = owner->owner.reader.readUInt32();

	switch (rstatus)
	{
	case TCP::Done:
		if (status == TCPStatus::ConnectSent) {
			status = TCPStatus::Connected;
			sendCallbackEvent(TCPEvent::Connected, ip);
		}
		else
			status = TCPStatus::Error;
		break;
	case TCP::Disconnected:
		status = TCPStatus::Disconnected;
		sendCallbackEvent(TCPEvent::Disconnected);
		break;
	case TCP::NameNotResolved:
		status = TCPStatus::Error;
		sendCallbackEvent(TCPEvent::HostNotFound);
		break;
	case TCP::NotReady:
	case TCP::Error:
	case TCP::Busy:
	default:
		status = TCPStatus::Error;
		sendCallbackEvent(TCPEvent::Error, ip);
		break;
	}
}

void TCPSock::parseTCPSendPacket() {
	TCP::RspStatus rstatus = (TCP::RspStatus)owner->owner.reader.readUInt8();
	size_t sended = (TCP::RspStatus)owner->owner.reader.readVarInt(); //TODO

	switch (rstatus)
	{
	//case TCP::NotReady: //TODO
	case TCP::Disconnected:
		status = TCPStatus::Disconnected;
		sendCallbackEvent(TCPEvent::Disconnected);
		break;
	case TCP::Error:
	case TCP::Busy:
		status = TCPStatus::Error;
		sendCallbackEvent(TCPEvent::Error);
		break;
	}
}

void TCPSock::parseTCPReceivePacket() {
	PacketReader& reader = owner->owner.reader;
	TCP::RspStatus rstatus = (TCP::RspStatus)reader.readUInt8();

	size_t size = reader.get_size_to_end();
	size_t free_size = tcp_receive_buf_size - receive_buf_pos;

	if (size > free_size)
		abort();

	reader.read(receive_buf + receive_buf_pos, size);
	receive_buf_pos += size;

	switch (rstatus)
	{
		//case TCP::NotReady: //TODO
	case TCP::Disconnected:
		status = TCPStatus::Disconnected;
		sendCallbackEvent(TCPEvent::Disconnected);
		break;
	case TCP::Error:
	case TCP::Busy:
		status = TCPStatus::Error;
		sendCallbackEvent(TCPEvent::Error);
		break;
	}
}

void TCPSock::parsePacket() {
	uint8_t act = owner->owner.reader.readUInt8();

	switch (act) {
	case TCP::Connect:
		parseTCPConnectPacket();
		break;
	case TCP::Send:
		parseTCPSendPacket();
		break;
	case TCP::Receive:
		parseTCPReceivePacket();
		break;
	}
}

void TCPSock::update() {
	switch (status)
	{
	case TCPSock::None:
		break;
	case TCPSock::InitPending:
		if (make_init_packet())
			status = TCPSock::Inited;
		break;
	case TCPSock::ConnectPending:
		if (make_connect_packet())
			status = TCPSock::ConnectSent;
		break;
	case TCPSock::ConnectSent:
		break;
	case TCPSock::Connected:
		break;
	case TCPSock::ClosingPending:
		if (make_disconnect_packet())
			return owner->TCPsock_remove(id);
		break;
	default:
		break;
	}

	if (receive_count)
		if (make_receive_packet())
			receive_count = 0;
}

void TCPSock::updateData() {
	if (status == TCPSock::Connected)
		send();
}

ssize_t TCPSock::write(const void* buf, size_t size) {
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

ssize_t TCPSock::read(void* buf, size_t size) {
	int used_size = receive_buf_pos;
	if (size > used_size)
		size = used_size;

	if (status == TCPStatus::Error)
		return -2;

	if (size == 0)
		return 0;

	memcpy(buf, receive_buf, size);

	if (used_size - size)
		memmove(receive_buf, receive_buf + size, used_size - size);

	receive_buf_pos -= size;

	receive_count += size;

	//update();

	return size;
}

void TCPSock::close() {
	status = ClosingPending;

	update();
}