#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <vmsys.h>
#include <vmbtcm.h>

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

enum bt_obex_statuses {
	GOEP_STATUS_SUCCESS,
	GOEP_STATUS_FAILED,
	GOEP_STATUS_PENDING,
	GOEP_STATUS_DISCONNECT,
	GOEP_STATUS_NO_CONNECT,
	GOEP_STATUS_BUSY			 = 11,
	GOEP_STATUS_NO_RESOURCES	 = 12,
	GOEP_STATUS_INVALID_PARM	 = 18,
	GOEP_STATUS_IN_PROGRESS		 = 19,
	GOEP_STATUS_NOT_SUPPORTED	 = 23,
	GOEP_STATUS_RESTRICTED		 = 20,
	GOEP_STATUS_PACKET_TOO_SMALL = 31,
	GOEP_STATUS_NO_SESSION		 = 32,
	GOEP_STATUS_SCO_REJECT		 = 25
};

enum opp_roles{
	OPP_ROLE_CLIENT = 1,
	OPP_ROLE_SERVER,
	OPP_ROLE_END
};

typedef enum goep_pkt_types{
	GOEP_FIRST_PKT = 0,
	GOEP_NORMAL_PKT,
	GOEP_FINAL_PKT,
	GOEP_SINGLE_PKT
};

typedef struct ilm_struct {
	VMUINT16 src_mod_id;
	VMUINT16 dest_mod_id;
	VMUINT16 sap_id;
	VMUINT16 msg_id;
} ilm_struct;

typedef struct
{
	VMUINT8 ref_count;
	VMUINT16 msg_len;
	VMUINT8 uuid[16];
	VMUINT8 uuid_len;
	VMUINT8 req_id;
	vm_srv_bt_cm_bt_addr bd_addr;
	VMUINT8 tp_type;
	VMUINT8* buf_ptr;
	VMUINT16 buf_size;
	VMBOOL auth_use;
	VMUINT8 passwd[16];
	VMUINT8 passwd_len;
	VMUINT8 realm[20];
	VMUINT8 realm_len;
} goep_connect_req_struct;

#define GOEP_MSG_COMM \
    VMUINT8 ref_count; \
    VMUINT16 msg_len; \
    VMUINT8 uuid[16];\
    VMUINT8 uuid_len;\
    VMUINT8 goep_conn_id\

typedef struct
{
	GOEP_MSG_COMM;

	VMUINT8 req_id;
	VMUINT8 rsp_code;
	VMUINT16 peer_mru;
	VMUINT32 cm_conn_id;
} goep_connect_rsp_struct;

typedef struct
{
	GOEP_MSG_COMM;

	VMUINT8 rsp_code;
	VMUINT8 reason; 
} goep_push_rsp_struct;

#define MAX_OBEX_PACKET_LENGTH 0x4000 

typedef VMUINT8 (*MSGHandler) (void* local_buf, int src_mod, ilm_struct* ilm);

extern void (*mmi_bt_obex_event_hdlr_init)(void);
extern void (*mmi_frm_set_protocol_event_handler)(VMUINT16 eventID, MSGHandler funcPtr, VMBOOL isMultiHandler);

extern void (*srv_opp_send_ilm)(VMUINT32 msg_id, void* local_para_p);

extern VMINT32(*srv_bt_cm_start_conn)(VMBOOL in_out, VMINT32 profile_id, void* dev_addr, VMCHAR* dev_name);
extern void (*srv_bt_cm_connect_ind)(VMUINT32 conn_id);
extern void (*srv_bt_cm_stop_conn)(VMUINT32 conn_id);

extern void* (*construct_local_para)(VMUINT16 local_para_size, VMINT32 direction);

extern VMUINT32 (*srv_opp_open)(VMUINT8 role);
extern VMUINT32 (*srv_opp_close)(VMUINT32 srv_hd);

extern void (*srv_oppc_send_push_req)(int goep_conn_id, int pkt_type, VMINT32 total_obj_len, VMWSTR obj_name, int obj_mime, VMUINT32* frag_ptr, VMUINT16 frag_len);
extern void (*srv_oppc_send_abort_req)(VMINT8 goep_conn_id);
extern void (*srv_oppc_send_disconnect_req)(VMINT8 goep_conn_id, int tpdisconn_flag);
extern VMINT32 (*srv_oppc_send_begin)(VMINT32 srv_hd, void* dst_dev, VMUINT8* buffer, VMUINT16 buf_size);
extern void (*srv_oppc_notify_app)(VMINT32 event_id, void* para);

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