#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <vmsys.h>
#include <vmbtcm.h>


VMBOOL bt_opp_preinit();
VMBOOL bt_opp_init();
VMBOOL bt_opp_connect(VMUINT8* mac);
VMBOOL bt_opp_deinit();






#define DEBUG_PRINTF(...) cprintf(__VA_ARGS__)

#ifdef WIN32
#define PLATFORM_ASSERT() DEBUG_PRINTF("WIN32 is not support for now\n"); return FALSE;
#else
#define PLATFORM_ASSERT() ;
#endif // WIN32


#ifdef __cplusplus
}
#endif