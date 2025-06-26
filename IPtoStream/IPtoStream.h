#pragma once
#include <stdint.h>
#include "PacketMaker.h"
#include "PacketReader.h"
#include "TCP.h"

class IPtoStream {
	friend class TCP;

	const uint8_t bits_for_type = 11;

	enum Types : uint8_t {
		TCP_T
	};

	PacketMaker writer;
	PacketReader reader;


	uint16_t getType(uint16_t type_id);
	uint16_t getId(uint16_t type_id);
	uint16_t makeTypeId(uint16_t type, uint16_t id);

public:
	IPtoStream();

	TCP tcp;
};

extern IPtoStream ipts;