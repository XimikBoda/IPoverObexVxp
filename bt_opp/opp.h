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

VMBOOL bt_opp_pre_init();

#ifdef __cplusplus
}
#endif