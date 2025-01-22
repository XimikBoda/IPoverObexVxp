#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <vmsys.h>

typedef VMINT(*vm_get_sym_entry_t)(char* symbol);
extern vm_get_sym_entry_t vm_get_sym_entry;

enum bt_spp_events {
	BT_SPP_ACTIVATE_REQ,
	BT_SPP_CONNECT_REQ,
	BT_SPP_SCO_CONNECT_REQ,
	BT_SPP_SEND_DATA_REQ,
	BT_SPP_GET_DATA_REQ,
	BT_SPP_DEACTIVATE_REQ,
	BT_SPP_DISCONNECT_REQ,
	BT_SPP_AUDIO_CONNECT_REQ,
	BT_SPP_AUDIO_DISCONNECT_REQ,
	BT_SPP_CONNECT_IND_RES,
	BT_SPP_AUTH_RSP,
	BT_SPP_UART_OWNER_CNF,
	BT_SPP_UART_PLUGOUT_CNF,
	BT_SPP_CONNECT_IND = 24,
	BT_SPP_CONNECT_IND_REQ,
	BT_SPP_SCO_CONNECT_IND,
	BT_SPP_DISCONNECT_IND,
	BT_SPP_ACTIVATE_CNF,
	BT_SPP_DEACTIVATE_CNF,
	BT_SPP_DISCONNECT_CNF,
	BT_SPP_AUTH_REQ,
	BT_SPP_AUDIO_CONNECT_CNF,
	BT_SPP_AUDIO_CONNECT_IND,
	BT_SPP_AUDIO_DISCONNECT_CNF,
	BT_SPP_AUDIO_DISCONNECT_IND,
	BT_SPP_SCO_RSSI_IND,
	BT_SPP_CONNECT_CNF,
	BT_SPP_UART_OWNER_IND,
};

typedef struct ilm_struct {
	VMUINT8 src_mod_id;
	VMUINT8 dest_mod_id;
	VMUINT8 sap_id;
	VMUINT8 msg_id;
} ilm_struct;

typedef struct
{
	VMUINT8 ref_count;
	VMUINT16 msg_len;
	VMUINT32 lap;
	VMUINT8 uap;
	VMUINT16 nap;
	VMUINT8* txBufPtr;
	VMUINT8* rxBufPtr;
	VMUINT32 txBufSize;
	VMUINT32 rxBufSize;
	VMUINT8 server_chnl_num;
	VMUINT16 uuid;
	VMUINT32 req_id;
	VMUINT8 sec_level;
} bt_spp_connect_req_struct;

typedef VMUINT8 (*MSGHandler) (void* local_buf, int src_mod, ilm_struct* ilm);

extern void (*mmi_bt_obex_event_hdlr_init)(void);
extern void (*mmi_frm_set_protocol_event_handler)(VMUINT16 eventID, MSGHandler funcPtr, VMBOOL isMultiHandler);

extern void* (*construct_local_para)(VMUINT16 local_para_size, VMINT32 direction);

extern VMINT32(*srv_bt_cm_start_conn)(VMBOOL in_out, VMINT32 profile_id, void* dev_addr, VMCHAR* dev_name);
extern void (*srv_bt_cm_connect_ind)(VMUINT32 conn_id);
extern void (*srv_bt_cm_stop_conn)(VMUINT32 conn_id);

extern void (*srv_opp_send_ilm)(VMUINT32 msg_id, void* local_para_p);

VMBOOL bt_preinit();
VMBOOL bt_spp_init();
VMBOOL bt_spp_connect(VMUINT8* mac);
VMBOOL bt_deinit();

#define DEBUG_PRINTF(...) cprintf(__VA_ARGS__)

#ifdef WIN32
#define PLATFORM_ASSERT() DEBUG_PRINTF("WIN32 is not support for now\n"); return FALSE;
#else
#define PLATFORM_ASSERT() ;
#endif // WIN32


#ifdef __cplusplus
}
#endif