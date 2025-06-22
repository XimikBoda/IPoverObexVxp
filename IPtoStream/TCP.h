#pragma once
#include <stdint.h>
#include <functional>

#include "ItemMng.h"

typedef class IPtoStream;

class TCP {
	friend class IPtoStream;

	IPtoStream &owner;
	uint8_t my_type;

	enum TCPAct : uint8_t {
		Connect,
		Disconnect
	};

	enum RspStatus : uint8_t {
		Done,
		NotReady,
		Disconnected,
		Error,
		Busy
	};

	TCP(IPtoStream &owner_, uint8_t type);

	class TCP_sock {
	public:
		char host[256] = {};
		uint16_t port = 0;
	};

	ItemsMngConst<TCP_sock, 10> TCPsocks;


	bool make_connect_packet(int id);

public:
	int connect(const char* host, uint16_t port, std::function<void(int handle, int event)> callback); //TODO add also raw callback without std::function
};