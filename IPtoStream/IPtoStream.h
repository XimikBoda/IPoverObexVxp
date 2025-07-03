#pragma once
#include <stdint.h>
#include "PacketMaker.h"
#include "PacketReader.h"
#include "TCP.h"

class IPtoStream {
	friend class TCP;
	friend class TCPSock;

	enum Types : uint8_t {
		TCP_T
	};

	PacketMaker writer;
	PacketReader reader;

	uint16_t type;
	uint16_t id;

	void parsePacket();

public:
	IPtoStream();

	void update();

	TCP tcp;
};

extern IPtoStream ipts;