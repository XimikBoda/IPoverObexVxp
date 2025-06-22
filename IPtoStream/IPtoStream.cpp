#include "IPtoStream.h"

#undef ipts

IPtoStream ipts;

uint16_t IPtoStream::getType(uint16_t type_id) {
	return type_id >> bits_for_type;
}

uint16_t IPtoStream::getId(uint16_t type_id) {
	return type_id & ((1 << bits_for_type) - 1);
}

uint16_t IPtoStream::makeTypeId(uint16_t type, uint16_t id) {
	return (type << bits_for_type) | (id & ((1 << bits_for_type) - 1));
}

inline IPtoStream::IPtoStream() : tcp(*this, TCP_T) {}
