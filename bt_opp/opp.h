#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <vmsys.h>

typedef VMINT(*vm_get_sym_entry_t)(char* symbol);
extern vm_get_sym_entry_t vm_get_sym_entry;


enum bt_obex_events{
	GOEP_REGISTER_SERVER_REQ,
	GOEP_REGISTER_SERVER_RSP,
	GOEP_DEREGISTER_SERVER_REQ,
	GOEP_DEREGISTER_SERVER_RSP,
	GOEP_CONNECT_IND,
	GOEP_CONNECT_RES,
	GOEP_PUSH_IND,
	GOEP_PUSH_RES,
	GOEP_PULL_IND,
	GOEP_PULL_RES,
	GOEP_SET_FOLDER_IND,
	GOEP_SET_FOLDER_RES,
	GOEP_ABORT_IND,
	GOEP_ABORT_RES,
	GOEP_AUTH_REQ,
	GOEP_AUTH_RSP,
	GOEP_REGISTER_CLIENT_REQ,
	GOEP_REGISTER_CLIENT_RSP,
	GOEP_DEREGISTER_CLIENT_REQ,
	GOEP_DEREGISTER_CLIENT_RSP,
	GOEP_CONNECT_REQ,
	GOEP_CONNECT_RSP,
	GOEP_PUSH_REQ,
	GOEP_PUSH_RSP,
	GOEP_PULL_REQ,
	GOEP_PULL_RSP,
	GOEP_SET_FOLDER_REQ,
	GOEP_SET_FOLDER_RSP,
	GOEP_ABORT_REQ,
	GOEP_ABORT_RSP,
	GOEP_DISCONNECT_REQ,
	GOEP_DISCONNECT_RSP,
	GOEP_DISCONNECT_IND,
	GOEP_DISCONNECT_RES,
	GOEP_TPDISCONNECT_REQ,
	GOEP_AUTH_IND,
	GOEP_AUTH_RES,
	GOEP_OPP_SUPPORTED_FORMATS_IND,
	GOEP_AUTHORIZE_IND,
	GOEP_AUTHORIZE_RES,
};

typedef struct ilm_struct {
	VMUINT8 src_mod_id;
	VMUINT8 dest_mod_id;
	VMUINT8 sap_id;
	VMUINT8 msg_id;
} ilm_struct;

typedef VMUINT8 (*MSGHandler) (void* local_buf, int src_mod, ilm_struct* ilm);

extern void (*mmi_bt_obex_event_hdlr_init)(void);
extern void (*mmi_frm_set_protocol_event_handler)(VMUINT16 eventID, MSGHandler funcPtr, VMBOOL isMultiHandler);

extern void (*srv_bt_cm_connect_ind)(VMUINT32 conn_id);
extern void (*srv_bt_cm_stop_conn)(VMUINT32 conn_id);

extern VMUINT32 (*srv_opp_open)(VMUINT8 role);
extern VMUINT32 (*srv_opp_close)(VMUINT32 srv_hd);

extern void (*srv_oppc_send_push_req)(int goep_conn_id, int pkt_type, VMINT32 total_obj_len, VMWSTR obj_name, int obj_mime, VMUINT32* frag_ptr, VMUINT16 frag_len);
extern void (*srv_oppc_send_abort_req)(VMINT8 goep_conn_id);
extern void (*srv_oppc_send_disconnect_req)(VMINT8 goep_conn_id, int tpdisconn_flag);
extern VMINT32 (*srv_oppc_send_begin)(VMINT32 srv_hd, void* dst_dev, VMUINT8* buffer, VMUINT16 buf_size);
extern void (*srv_oppc_notify_app)(VMINT32 event_id, void* para);

VMBOOL bt_opp_pre_init();

#ifdef __cplusplus
}
#endif