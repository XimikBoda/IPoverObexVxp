#pragma once
#include <stdint.h>
#include <cstddef>


class PacketReader {
	uint8_t* receive_buf = 0;
	size_t pos = 0;

	size_t receive_buf_size = 0;
	size_t packet_buf_size = 0;
public:

	static uint16_t getType(uint16_t type_id);
	static uint16_t getId(uint16_t type_id);

	bool check_receive();

	void read(void *buf, size_t len);
	uint8_t readUInt8();
	uint16_t readUInt16();
	uint32_t readUInt32();
	uint32_t readVarInt();
	//std::string readString();

	void end();
};