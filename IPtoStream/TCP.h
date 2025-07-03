#pragma once
#include <stdint.h>

#include "ItemMng.h"

//#define USE_STD_FUNCTION

enum TCPEvent : uint8_t {
	Connected,
	Disconnected,
	HostNotFound,
	Error
};

#ifdef  USE_STD_FUNCTION
#include <functional>
typedef std::function<void(int id, TCPEvent event)> tcp_callback_t;
#else
typedef void (*tcp_callback_t)(int id, TCPEvent event);
#endif //  USE_STD_FUNCTION


const size_t tcp_receive_buf_size = 1024;
const size_t tcp_send_buf_size = 1024;

class TCP_sock {
public:

	char host[256] = {};
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

	void sendCallbackEvent(TCPEvent event);
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

class TCP {
	friend class IPtoStream;
	friend class TCP_sock;

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

	ItemsMngConst<TCP_sock, 10> TCPsocks;


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