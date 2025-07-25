#pragma once
#include <stdint.h>
#include <cstddef>

const size_t log_msgs_buf_len = 1024 * 10;

class Log {
public:
	enum class Level : uint8_t {
		None,
		Trace,
		Debug,
		Info,
		Warning,
		Error,
		Critical,
	};

private:
	friend class IPtoStream;

	class IPtoStream& owner;
	uint8_t my_type;

	uint8_t buf[log_msgs_buf_len] = {};
	uint32_t buf_pos = 0;

	enum class Act : uint8_t {
		Log,
	};

	

	Log(IPtoStream& owner_, uint8_t type);

	void putBuf(const void* buf, size_t len); //todo move it
	void putUInt8(uint8_t val);
	void putUInt16(uint16_t val);
	void putVarInt(int32_t val);
	void putString(const char* str);
	void setUInt16(int32_t pos, uint16_t val);
	uint16_t getpUInt16(int32_t pos);

	void update();

public:
	bool post(Level level, const char* name, const char* msg);
	bool pprintf(Level level, const char* name, char const* const format, ...);
};