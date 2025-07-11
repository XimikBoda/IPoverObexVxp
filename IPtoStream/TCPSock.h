#pragma once
#include "ItemMng.h"
#include <stdint.h>

enum class TCPEvent : uint8_t {
	Connected,
	Disconnected,
	HostNotFound,
	Error
};

#ifdef  USE_STD_FUNCTION
#include <functional>
typedef std::function<void(int id, TCPEvent event, uint32_t val)> tcp_callback_t;
#else
typedef void (*tcp_callback_t)(int id, TCPEvent event, uint32_t val);
#endif //  USE_STD_FUNCTION


const size_t tcp_receive_buf_size = 1024;
const size_t tcp_send_buf_size = 1024;

class TCPSock {
public:

	char host[256] = {};
	uint32_t ip = 0; // getted after connecting
	uint16_t port = 0;
	uint16_t id = 0;
	uint16_t type_id = 0;
	tcp_callback_t callback = 0;

	uint8_t receive_buf[tcp_receive_buf_size] = {};
	uint8_t send_buf[tcp_send_buf_size] = {};
	size_t receive_buf_pos = 0;
	size_t send_buf_pos = 0;

	size_t receive_count = 0;

	class TCP* owner;

	enum TCPStatus : uint8_t {
		None,
		InitPending,
		Inited,
		ConnectPending,
		ConnectSent,
		Connected,
		Disconnected,
		Error,
		ClosingPending
	};

	TCPStatus status = None;

	void sendCallbackEvent(TCPEvent event, uint32_t val = 0);
	void send();

	bool make_init_packet();
	bool make_connect_packet();
	bool make_receive_packet();
	bool make_disconnect_packet();

	void parseTCPConnectPacket();
	void parseTCPSendPacket();
	void parseTCPReceivePacket();
	void parsePacket();
	void update(); // small inform packets
	void updateData(); // data packets

	ssize_t write(const void* buf, size_t size);
	ssize_t read(void* buf, size_t size);
	void close();
};