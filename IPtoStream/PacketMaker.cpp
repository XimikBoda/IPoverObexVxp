#include "PacketMaker.h"
#include <string.h>

void PacketMaker::init(uint16_t type_id) {
	size = 0;
	putUInt16(0);
	putUInt16(type_id);
}

void PacketMaker::update_size() {
	uint16_t size16 = size;
	memcpy(vec, &size16, 2);
}

void PacketMaker::putBuf(const void* buf, size_t len) {
	memcpy(vec + size, buf, len);
	size += len;
}

void PacketMaker::putUInt8(uint8_t val) {
	vec[size] = val;
	size++;
}

void PacketMaker::putUInt16(uint16_t val) {
	putBuf(&val, sizeof(val));
}

void PacketMaker::putUInt32(uint32_t val) {
	putBuf(&val, sizeof(val));
}
