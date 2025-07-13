#pragma once
#include "ItemMng.h"
#include "TCPSock.h"
#include "TCPListener.h"
#include <stdint.h>

class TCP {
	friend class IPtoStream;
	friend class TCPSock;
	friend class TCPListener;

	class IPtoStream &owner;
	uint8_t my_type;

	enum TCPAct : uint8_t {
		Init,
		Connect,
		Send,
		Receive,
		Disconnect
	};

	enum TCPLAct : uint8_t {
		Bind,
		Accept,
		Close,
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
	ItemsMngConst<TCPListener, 10> TCPlisteners;


	void parsePacket();
	void parseListenerPacket();

	void updateData();
	void update();

	void TCPsock_remove(uint16_t id);
	void TCPlistener_remove(uint16_t id);

public:
	int init(tcp_callback_t callback); // for bind
	int connect(const char* host, uint16_t port, tcp_callback_t callback);
	ssize_t write(int id, const void* buf, size_t size);
	ssize_t read(int id, void* buf, size_t size);
	void close(int id);

	int lbind(uint16_t port, tcpl_callback_t callback);
	void laccept(int id, uint16_t tcpsock_id);
	void lclose(int id);
};