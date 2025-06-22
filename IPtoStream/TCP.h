#pragma once
#include <stdint.h>
#include <functional>

class TCP {
public:
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

private:
public:
	int connect(const char* host, uint16_t port, std::function<void(int handle, int event)> callback);
};