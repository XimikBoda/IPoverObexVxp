#pragma once
#include <stdint.h>
#include <cstddef>
#include <vmsys.h>

const size_t PM_max_size = 1024; 

class PacketMaker {
	friend class IPtoStream;

	uint8_t vec[PM_max_size] = {}; //TODO add protection of overflow
	size_t size = 0;
	bool done = true;

	void update_size();

	VMUINT32 (*s_write)(const void* buf, VMUINT32 size) = 0;
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