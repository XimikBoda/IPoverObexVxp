#include "TCPListener.h"
#include "TCP.h"
#include "IPtoStream.h"
#include <cstring>
#include <console.h>

void TCPListener::sendCallbackEvent(TCPLEvent event) {
	if (callback)
		callback(id, event);
}

bool TCPListener::make_bind_packet() {
	auto& writer = owner->owner.writer;

	if (!writer.available())
		return false;

	writer.init(type_id);
	writer.putUInt8(TCP::Bind);
	writer.putUInt16(port);
	writer.send();

	return true;
}

bool TCPListener::make_accept_packet() {
	auto& writer = owner->owner.writer;

	if (!writer.available())
		return false;

	writer.init(type_id);
	writer.putUInt8(TCP::Accept);
	writer.putUInt16(tcpsock_id);
	writer.send();

	return true;
}

bool TCPListener::make_close_packet() {
	auto& writer = owner->owner.writer;

	if (!writer.available())
		return false;

	writer.init(type_id);
	writer.putUInt8(TCP::Close);
	writer.send();

	return true;
}

void TCPListener::parseBindPacket() {
	TCP::RspStatus rstatus = (TCP::RspStatus)owner->owner.reader.readUInt8();

	switch (rstatus)
	{
	case TCP::Done:
		if (status == TCPListener::BindSent) {
			status = TCPListener::Binded;
			sendCallbackEvent(TCPLEvent::Binded);
		}
		else
			status = TCPListener::Error;
		break;
	case TCP::Disconnected:
	case TCP::NameNotResolved:
	case TCP::NotReady:
	case TCP::Error:
	case TCP::Busy:
	default:
		status = TCPListener::Error;
		sendCallbackEvent(TCPLEvent::Error);
		break;
	}
}

void TCPListener::parseAcceptPacket() {
	TCP::RspStatus rstatus = (TCP::RspStatus)owner->owner.reader.readUInt8();

	switch (rstatus)
	{
	case TCP::Done:
		if (status == TCPListener::AcceptSent) {
			status = TCPListener::Accepted;
			sendCallbackEvent(TCPLEvent::Accepted);
		}
		else
			status = TCPListener::Error;
		break;
	case TCP::Disconnected:
	case TCP::NameNotResolved:
	case TCP::NotReady:
	case TCP::Error:
	case TCP::Busy:
	default:
		status = TCPListener::Error;
		sendCallbackEvent(TCPLEvent::Error);
		break;
	}
}

void TCPListener::parsePacket() {
	uint8_t act = owner->owner.reader.readUInt8();

	switch (act) {
	case TCP::Bind:
		parseBindPacket();
		break;
	case TCP::Accept:
		parseAcceptPacket();
		break;
	}
}

void TCPListener::update() {
	switch (status)
	{
	case TCPListener::BindPending:
		if (make_bind_packet())
			status = TCPListener::BindSent;
		break;
	case TCPListener::AcceptPending:
		if (make_accept_packet())
			status = TCPListener::AcceptSent;
		break;
	case TCPListener::ClosingPending:
		if (make_close_packet())
			return owner->TCPlistener_remove(id);
		break;
	default:
		break;
	}
}

void TCPListener::accept(uint16_t tcpsock_id) {
	this->tcpsock_id = tcpsock_id;
	status = AcceptPending;

	update();
}

void TCPListener::close() {
	status = ClosingPending;

	update();
}