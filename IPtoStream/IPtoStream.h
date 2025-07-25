#pragma once
#include <stdint.h>
#include "PacketMaker.h"
#include "PacketReader.h"
#include "TCP.h"
#include "Log.h"

enum class StreamType {
	BT,
	TCP
};

class IPtoStream {
	friend class Log;
	friend class TCP;
	friend class TCPSock;
	friend class TCPListener;

	enum Types : uint8_t {
		LOG_T,
		TCP_T,
		TCP_LISTENER_T,
	};

	PacketMaker writer;
	PacketReader reader;

	uint16_t type;
	uint16_t id;

	StreamType stype;

	void parsePacket();

public:
	IPtoStream();

	void init(StreamType type);
	void connectBT(uint8_t mac[6]);
	void connectTCP(const char *adr);

	void update();

	void disconect();
	void quit();

	TCP tcp;
	Log log;
};

extern IPtoStream ipts;