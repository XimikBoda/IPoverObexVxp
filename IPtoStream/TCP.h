#pragma once
#include "ItemMng.h"
#include "TCPSock.h"
#include <stdint.h>

class TCP {
	friend class IPtoStream;
	friend class TCPSock;

	class IPtoStream &owner;
	uint8_t my_type;

	enum TCPAct : uint8_t {
		Init,
		Connect,
		Send,
		Receive,
		Disconnect
	};

	enum RspStatus : uint8_t {
		Done,
		NotReady,
		Disconnected,
		Error,
		Busy,
		NameNotResolved
	};

	TCP(IPtoStream &owner_, uint8_t type);

	ItemsMngConst<TCPSock, 10> TCPsocks;


	void parsePacket();

	void updateData();
	void update();

public:
	int init(tcp_callback_t callback); // for bind
	int connect(const char* host, uint16_t port, tcp_callback_t callback);
	ssize_t write(int id, const void* buf, size_t size);
	ssize_t read(int id, void* buf, size_t size);
	void close(int id);
};