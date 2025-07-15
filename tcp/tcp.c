#include <vmsys.h>
#include <vmchset.h>
#include <console.h>
#include <string.h>
#include <vmsock.h>
#include "stcp.h"

static VMINT32 sock_id = -1;

static void* send_buf = 0;
static void* receive_buf = 0;

static VMINT32 send_buf_pos = 0;
static VMINT32 receive_buf_pos = 0;

static VMBOOL is_connected = 0;

#define MALLOC_ASSERT(x) if (!x) return FALSE;

VMBOOL tcp_init() {
	send_buf = vm_malloc(SEND_BUF);
	MALLOC_ASSERT(send_buf);
	receive_buf = vm_malloc(RECEIVE_BUF);
	MALLOC_ASSERT(receive_buf);

	send_buf_pos = 0;
	receive_buf_pos = 0;
	
	return TRUE;
}

void tcp_callback(VMINT handle, VMINT event) {
	if (handle != sock_id)
		return;

	switch (event) {
	case VM_TCP_EVT_CONNECTED:
		is_connected = TRUE;
		break;
	case VM_TCP_EVT_CAN_WRITE:
		break;
	case VM_TCP_EVT_CAN_READ:
		break;
	case VM_TCP_EVT_PIPE_BROKEN:
	case VM_TCP_EVT_HOST_NOT_FOUND:
	case VM_TCP_EVT_PIPE_CLOSED:
		is_connected = FALSE;
		break;
	default:
		break;
	}
}

VMBOOL tcp_connect(const char* addr, VMUINT16 port) {
	sock_id = vm_tcp_connect(addr, port, 1, tcp_callback);
	if (sock_id >= 0)
		return TRUE;
}

VMBOOL tcp_disconnect() {
	if (sock_id >= 0) {
		vm_tcp_close(sock_id);
		sock_id = -1;
		is_connected = FALSE;
	}
	return TRUE;
}

VMBOOL tcp_deinit() {
	return TRUE;
}

VMBOOL tcp_is_connected() {
	return is_connected;
}

void tcp_flush() {
	if (RECEIVE_BUF - send_buf_pos) {
		int res = vm_tcp_read(sock_id, (char*)send_buf + send_buf_pos, RECEIVE_BUF - send_buf_pos);
		if (res >= 0)
			send_buf_pos += res;
	}

	if (send_buf_pos) {
		int res = vm_tcp_write(sock_id, (char*)send_buf + send_buf_pos, send_buf_pos);
		if (res >= 0) {
			memmove(send_buf, (char*)send_buf + res, send_buf_pos - res);
			send_buf_pos -= res;
		}
	}
}

VMUINT32 tcp_get_free_size() {
	return SEND_BUF - send_buf_pos;
}

VMUINT32 tcp_write(const void* buf, VMUINT32 size) {
	VMUINT32 free_size = bt_opp_get_free_size();
	if (size > free_size)
		size = free_size;

	memcpy((char*)send_buf + send_buf_pos, buf, size);
	send_buf_pos += size;

	bt_opp_flush();

	return size;
}


VMUINT32 tcp_read(void* buf, VMUINT32 size) {
	VMUINT32 used_size = receive_buf_pos;
	if (size > used_size)
		size = used_size;

	memcpy(buf, receive_buf, size);
	if (used_size - size)
		memmove(receive_buf, (char*)receive_buf + size, used_size - size);

	receive_buf_pos -= size;

	bt_opp_flush();

	return size;
}

void tcp_set_as_readed(VMUINT32 size) {
	VMUINT32 used_size = receive_buf_pos;
	if (size > used_size)
		abort();

	if (used_size - size)
		memmove(receive_buf, (char*)receive_buf + size, used_size - size);

	receive_buf_pos -= size;

	bt_opp_flush();
}

void* tcp_get_receive_buf() {
	return receive_buf;
}

VMUINT32 tcp_get_receive_size() {
	return receive_buf_pos;
}

