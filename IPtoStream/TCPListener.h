#pragma once
#include "ItemMng.h"
#include <stdint.h>

enum class TCPLEvent : uint8_t {
	Binded,
	Accepted,
	Error,
};

#ifdef  USE_STD_FUNCTION
#include <functional>
typedef std::function<void(int id, TCPLEvent event)> tcpl_callback_t;
#else
typedef void (*tcpl_callback_t)(int id, TCPLEvent event);
#endif //  USE_STD_FUNCTION


class TCPListener {
public:

	tcpl_callback_t callback = 0;

	uint16_t port = 0;
	uint16_t tcpsock_id = 0;

	uint16_t id = 0;
	uint16_t type_id = 0;

	class TCP* owner;

	enum TCPListenerStatus : uint8_t {
		None,
		BindPending,
		BindSent,
		Binded,
		AcceptPending,
		AcceptSent,
		Accepted = Binded,
		Error = AcceptSent + 1,
		ClosingPending
	};

	TCPListenerStatus status = None;

	void sendCallbackEvent(TCPLEvent event);

	bool make_bind_packet();
	bool make_accept_packet();
	bool make_close_packet();

	void parseBindPacket();
	void parseAcceptPacket();
	void parsePacket();
	void update();

	void accept(uint16_t tcpsock_id);
	void close();
};