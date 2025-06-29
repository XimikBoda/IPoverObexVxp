#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <vmsys.h>
#include <vmbtcm.h>


VMBOOL bt_opp_preinit();
VMBOOL bt_opp_init();
VMBOOL bt_opp_connect(VMUINT8* mac);
VMBOOL bt_opp_disconnect();
VMBOOL bt_opp_deinit();

VMBOOL bt_opp_is_connected();
VMUINT32 bt_opp_get_free_size();
VMUINT32 bt_opp_write(const void* buf, VMUINT32 size); // For now only one stream
VMUINT32 bt_opp_read(void* buf, VMUINT32 size); // For now only one stream
void	 bt_opp_set_as_readed(VMUINT32 size); // For now only one stream
void*	 bt_opp_get_receive_buf();
VMUINT32 bt_opp_get_receive_size();
void bt_opp_flush();



//#define REGISTER_CONN 

#define OBEX_SEND_BUF (10*1024) // Size of data in one obex package
#define SEND_BUF (64*1024) // Size for data not yet sent
#define RECEIVE_BUF (64*1024) // Size for receiving data

#define DEBUG_PRINTF(...) cprintf(__VA_ARGS__)

#ifdef WIN32
#define PLATFORM_ASSERT() DEBUG_PRINTF("WIN32 is not support for now\n"); return FALSE;
#else
#define PLATFORM_ASSERT() ;
#endif // WIN32


#ifdef __cplusplus
}
#endif