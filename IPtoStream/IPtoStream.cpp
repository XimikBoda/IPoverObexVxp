#include "IPtoStream.h"
#include <console.h>

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

void IPtoStream::update() {
	if (reader.check_receive())
		parsePacket();


	tcp.update();

	tcp.updateData();
}
