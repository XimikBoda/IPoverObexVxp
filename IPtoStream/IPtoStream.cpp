#include "IPtoStream.h"
#include <console.h>
#include <opp.h>
#include <stcp.h>

#undef ipts

IPtoStream ipts;

void IPtoStream::parsePacket() {
	uint16_t type_id = reader.readUInt16();
	type = reader.getType(type_id);
	id = reader.getId(type_id);

	//cprintf("type_id %d -> (type %d, id %d)\n", type_id, type, id);

	switch (type) {
	case TCP_T:
		tcp.parsePacket();
		break;
	case TCP_LISTENER_T:
		tcp.parseListenerPacket();
		break;
	default:
		break;
	}

	reader.end();
}

inline IPtoStream::IPtoStream() : tcp(*this, TCP_T) {}

void IPtoStream::init(StreamType type) {
	stype = type;

	switch (stype) {
	case StreamType::BT:
		bt_opp_preinit();
		bt_opp_init();

		writer.s_write = bt_opp_write;
		reader.s_get_receive_size = bt_opp_get_receive_size;
		reader.s_get_receive_buf = bt_opp_get_receive_buf;
		reader.s_set_as_readed = bt_opp_set_as_readed;
		break;
	case StreamType::TCP:
		tcp_init();

		writer.s_write = tcp_write;
		reader.s_get_receive_size = tcp_get_receive_size;
		reader.s_get_receive_buf = tcp_get_receive_buf;
		reader.s_set_as_readed = tcp_set_as_readed;
		break;
	}
}

void IPtoStream::connectBT(uint8_t mac[6]) {
	bt_opp_connect(mac);
}

void IPtoStream::update() {
	switch (stype) {
	case StreamType::BT:
		bt_opp_flush();
		break;
	case StreamType::TCP:
		tcp_flush();
		break;
	}

	if (reader.check_receive())
		parsePacket();

	tcp.update();

	tcp.updateData();
}

void IPtoStream::disconect() {
	switch (stype) {
	case StreamType::BT:
		bt_opp_disconnect();
		break;
	case StreamType::TCP:
		break;
	}
}

void IPtoStream::quit() {
	disconect();

	switch (stype) {
	case StreamType::BT:
		bt_opp_deinit();
		break;
	case StreamType::TCP:
		break;
	}
}
