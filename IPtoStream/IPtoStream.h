#pragma once
#include <stdint.h>
#include "PacketMaker.h"
#include "TCP.h"

class IPtoStream {
	const uint8_t bits_for_type = 11;

	enum Types : uint8_t {
		TCP_T
	};

	PacketMaker writer;

public:

	TCP tcp;
};

extern IPtoStream ipts;