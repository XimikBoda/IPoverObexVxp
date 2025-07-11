#pragma once
#include <stdint.h>
#include <cstddef>

const size_t PM_max_size = 1024; 

class PacketMaker {
	uint8_t vec[PM_max_size] = {}; //TODO add protection of overflow
	size_t size = 0;
	bool done = true;

	void update_size();

public:
	static uint16_t makeTypeId(uint16_t type, uint16_t id);

	void init(uint16_t type_id);

	void putBuf(const void* buf, size_t len);
	void putUInt8(uint8_t val);
	void putUInt16(uint16_t val);
	void putUInt32(uint32_t val);

	void putVarInt(int32_t val);

	void putString(const char* str);

	void send();

	bool available();

	size_t free_size();
};