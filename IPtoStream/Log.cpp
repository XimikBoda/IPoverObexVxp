#include "Log.h"
#include "IPtoStream.h"
#include <cstring>
#include <console.h>

static const int SEGMENT_BITS = 0x7F;
static const int CONTINUE_BIT = 0x80;

void Log::putBuf(const void* buf_, size_t len) {
	memcpy(buf + buf_pos, buf_, len);
	buf_pos += len;
}

void Log::putUInt8(uint8_t val) {
	buf[buf_pos++] = val;
}

void Log::putUInt16(uint16_t val) {
	putBuf(&val, sizeof(val));
}

void Log::setUInt16(int32_t pos, uint16_t val) {
	memcpy(buf + pos, &val, 2);
}

uint16_t Log::getpUInt16(int32_t pos) {
	uint16_t val = 0;
	memcpy(&val, buf + pos, 2);
	return val;
}

void Log::putVarInt(int32_t val) {
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

void Log::putString(const char* str) {
	if (!str)
		return putVarInt(0);

	size_t len = strlen(str);
	putVarInt(len);
	putBuf(str, len);
}

Log::Log(IPtoStream& owner_, uint8_t type) : owner(owner_), my_type(type) {}

void Log::update() {
	auto& writer = owner.writer;
	size_t fpos = 0;

	while (fpos < buf_pos) {
		if (!writer.available())
			break;

		writer.init(my_type);
		writer.putUInt8((uint8_t)Act::Log);

		size_t size = getpUInt16(fpos);

		writer.putBuf(buf + 2, size - 2);
		writer.send();

		fpos += size;
	}

	memmove(buf, buf + fpos, buf_pos - fpos);
	buf_pos -= fpos;
}

bool Log::post(Level level, const char* name, const char* msg) {
	uint32_t free_buf_size = log_msgs_buf_len - buf_pos;

	if (!msg)
		return false;

	uint32_t needed = 3;
	if (name)
		needed += 4 + strlen(name);

	needed += 4 + strlen(msg);

	if (needed > free_buf_size)
		return false;


	uint32_t spos = buf_pos;
	putUInt16(0);
	putUInt8((uint8_t)level);
	putString(name);
	putString(msg);
	setUInt16(spos, buf_pos - spos); //size

	update();

	return true;
}

#include <stdarg.h>

extern "C" int vm_vsprintf(char* buf, const char* fmt, va_list args);

bool Log::pprintf(Level level, const char* name, char const* const format, ...) {
	static char buf[1024 * 2];

	va_list aptr;

	va_start(aptr, format);
	int ret = vm_vsprintf(buf, format, aptr);
	va_end(aptr);

	return post(level, name, buf);
}

