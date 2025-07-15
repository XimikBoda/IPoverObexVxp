#pragma once
#include <vmsys.h>

#ifdef __cplusplus
extern "C" {
#endif


VMBOOL tcp_init();
VMBOOL tcp_connect(const char* addr, VMUINT16 port);
VMBOOL tcp_disconnect();
VMBOOL tcp_deinit();

VMBOOL	 tcp_is_connected();
VMUINT32 tcp_get_free_size();
VMUINT32 tcp_write(const void* buf, VMUINT32 size); // For now only one stream
VMUINT32 tcp_read(void* buf, VMUINT32 size); // For now only one stream
void	 tcp_set_as_readed(VMUINT32 size); // For now only one stream
void*	 tcp_get_receive_buf();
VMUINT32 tcp_get_receive_size();
void	 tcp_flush();

#define SEND_BUF (64*1024) // Size for data not yet sent
#define RECEIVE_BUF (64*1024) // Size for receiving data

#define DEBUG_PRINTF(...) cprintf(__VA_ARGS__)


#ifdef __cplusplus
}
#endif