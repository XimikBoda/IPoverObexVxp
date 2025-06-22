#include "PacketMaker.h"
#include <string.h>
#include <opp.h>

static const int SEGMENT_BITS = 0x7F;
static const int CONTINUE_BIT = 0x80;

void PacketMaker::update_size() {
	uint16_t size16 = size;
	memcpy(vec, &size16, 2);
}

void PacketMaker::init(uint16_t type_id) {
	size = 0;
	done = false;
	putUInt16(0);
	putUInt16(type_id);
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

void PacketMaker::putVarInt(int32_t val) {
	uint32_t value = val;
	while (true) {
		if ((value & ~SEGMENT_BITS) == 0) {
			putUInt8(value & 0xFF);
			return;
		}

		putUInt8(((value & SEGMENT_BITS) | CONTINUE_BIT) & 0xFF);;

		value >>= 7;
	}
}

void PacketMaker::putString(const char* str) {
	size_t len = strlen(str);
	putVarInt(len);
	putBuf(str, len);
}

void PacketMaker::send() {
	update_size();

	bt_opp_write(vec, size);
	bt_opp_flush();

	done = true;
}
