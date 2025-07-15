#pragma once
#include <stdint.h>
#include <cstddef>
#include <vmsys.h>


class PacketReader {
	friend class IPtoStream;
	uint8_t* receive_buf = 0;
	size_t pos = 0;

	size_t receive_buf_size = 0;
	size_t packet_buf_size = 0;

	VMUINT32 (*s_get_receive_size)();
	void* (*s_get_receive_buf)();
	void (*s_set_as_readed)(VMUINT32 size);

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

	size_t get_size_to_end();

	void end();
};