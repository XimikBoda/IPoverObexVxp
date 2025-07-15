#include "PacketReader.h"
#include <string.h>

static const int SEGMENT_BITS = 0x7F;
static const int CONTINUE_BIT = 0x80;

static const uint8_t bits_for_type = 11;

uint16_t PacketReader::getType(uint16_t type_id) {
	return type_id >> bits_for_type;
}

uint16_t PacketReader::getId(uint16_t type_id) {
	return type_id & ((1 << bits_for_type) - 1);
}

bool PacketReader::check_receive() {
	receive_buf_size = s_get_receive_size();
	if (receive_buf_size < 3)
		return false;

	receive_buf = (uint8_t*)s_get_receive_buf();
	if (!receive_buf)
		return false;

	pos = 0;
	packet_buf_size = readUInt16();
	if (packet_buf_size > receive_buf_size)
		return false;

	return true;
}

void PacketReader::read(void* buf, size_t len) {
	memcpy(buf, receive_buf + pos, len);
	pos += len;
}

uint8_t PacketReader::readUInt8() {
	return ((uint8_t*)receive_buf)[pos++];
}

uint16_t PacketReader::readUInt16() {
	uint16_t val = 0;
	read(&val, 2);
	return val;
}

uint32_t PacketReader::readUInt32() {
	uint32_t val = 0;
	read(&val, 4);
	return val;
}

uint32_t PacketReader::readVarInt() {
	int32_t value = 0;
	uint8_t position = 0;
	uint8_t currentByte;

	while (true) {
		currentByte = readUInt8();
		value |= ((currentByte & SEGMENT_BITS) << position);

		if ((currentByte & CONTINUE_BIT) == 0) break;

		position += 7;
	}

	return value;
}

size_t PacketReader::get_size_to_end() {
	return packet_buf_size - pos;
}

void PacketReader::end() {
	s_set_as_readed(packet_buf_size);
}