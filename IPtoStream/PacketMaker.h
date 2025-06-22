#pragma once
#include <stdint.h>
#include <cstddef>

const size_t PM_max_size = 1024; 

class PacketMaker {
	uint8_t vec[PM_max_size] = {}; //TODO add protection of overflow
	size_t size = 0;
public:
	void init(uint16_t type_id);
	void update_size();

	void putBuf(const void* buf, size_t len);
	void putUInt8(uint8_t val);
	void putUInt16(uint16_t val);
	void putUInt32(uint32_t val);
};