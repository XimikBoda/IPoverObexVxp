#include <vmsys.h>
#include <vmbtcm.h>
#include <console.h>
#include <string.h>
#include "opp.h"

extern VMUINT32 bt_obex_events_base;
#define FIX_OBEX_EVENT(x) ((x) + bt_obex_events_base)

static VMINT32 opp_hndl = -1;
static VMINT32 conn_id = -1;

static void* buf_bt = 0;


static const char* bt_pw_st[4] = {
	"On", "Off", "Switching on", "Switching off"
};

void bt_mac_conv(VMUINT8* out, VMUINT8* in) { //TODO move to another place
	out[0] = in[5], out[1] = in[4], out[2] = in[3], out[3] = 0x00;
	out[4] = in[2], out[5] = 0x0F;  // 5th byte is supposedly an alignment byte.
	out[6] = in[1], out[7] = in[0];
}

static void print_hex(const char* s, unsigned char* h, int l) {
	DEBUG_PRINTF(s);
	for (int i = 0; i < l; ++i)
		DEBUG_PRINTF("%02X ", h[i]);
	DEBUG_PRINTF("\n");
}

static void oppc_connect_rsp_handler(void* msg) {
}

static void oppc_push_rsp_handler(void* msg) {
}

static VMUINT8 opp_event_handler(int msg_id, void* msg) {
	DEBUG_PRINTF("bt_spp_msg_event(%d)\n", msg_id);
	switch (msg_id - bt_obex_events_base) {
	case GOEP_CONNECT_RSP:
		oppc_connect_rsp_handler(msg);
		break;
	case GOEP_PUSH_RSP:
		oppc_push_rsp_handler(msg);
		break;
	}
}

static VMUINT8 opp_msg_handler(void* msg, int src_mod, ilm_struct* ilm) {
	opp_event_handler(ilm->msg_id, msg);
}

static void oppc_send_connect_req(VMINT conn_id, VMUINT8 buf, VMUINT16 buf_size, vm_srv_bt_cm_bt_addr *addr){
	goep_connect_req_struct* req;

	DEBUG_PRINTF("before construct_local_para \n");
	req = (goep_connect_req_struct*)construct_local_para(sizeof(*req), 0);

	req->uuid[0] = '\0';
	req->uuid_len = 0;
	req->req_id = conn_id;
	req->bd_addr = *addr;
	req->tp_type = 0;
	req->buf_ptr = buf;
	req->buf_size = buf_size;
	req->auth_use = 0;
	req->passwd_len = 0;
	req->realm_len = 0;

	DEBUG_PRINTF("before srv_opp_send_ilm \n");
	srv_opp_send_ilm(FIX_OBEX_EVENT(GOEP_CONNECT_REQ), req);
	DEBUG_PRINTF("After srv_opp_send_ilm \n");
}


VMBOOL bt_opp_init() {
	PLATFORM_ASSERT();

	DEBUG_PRINTF("BT statud: %s\n", bt_pw_st[vm_btcm_get_power_status()]);

	opp_hndl = srv_opp_open(OPP_ROLE_CLIENT);
	DEBUG_PRINTF("opp_hndl = %d\n", opp_hndl);
	//if (opp_hndl < 0)
	//	return FALSE;

	mmi_frm_set_protocol_event_handler(FIX_OBEX_EVENT(GOEP_REGISTER_SERVER_RSP), opp_msg_handler, 0);
	mmi_frm_set_protocol_event_handler(FIX_OBEX_EVENT(GOEP_DEREGISTER_SERVER_RSP), opp_msg_handler, 0);
	mmi_frm_set_protocol_event_handler(FIX_OBEX_EVENT(GOEP_CONNECT_IND), opp_msg_handler, 0);
	mmi_frm_set_protocol_event_handler(FIX_OBEX_EVENT(GOEP_PUSH_IND), opp_msg_handler, 0);
	mmi_frm_set_protocol_event_handler(FIX_OBEX_EVENT(GOEP_PULL_IND), opp_msg_handler, 0);
	mmi_frm_set_protocol_event_handler(FIX_OBEX_EVENT(GOEP_SET_FOLDER_IND), opp_msg_handler, 0);
	mmi_frm_set_protocol_event_handler(FIX_OBEX_EVENT(GOEP_ABORT_IND), opp_msg_handler, 0);
	mmi_frm_set_protocol_event_handler(FIX_OBEX_EVENT(GOEP_AUTHORIZE_IND), opp_msg_handler, 0);
	mmi_frm_set_protocol_event_handler(FIX_OBEX_EVENT(GOEP_CONNECT_RSP), opp_msg_handler, 0);
	mmi_frm_set_protocol_event_handler(FIX_OBEX_EVENT(GOEP_PUSH_RSP), opp_msg_handler, 0);
	mmi_frm_set_protocol_event_handler(FIX_OBEX_EVENT(GOEP_PULL_RSP), opp_msg_handler, 0);
	mmi_frm_set_protocol_event_handler(FIX_OBEX_EVENT(GOEP_SET_FOLDER_RSP), opp_msg_handler, 0);
	mmi_frm_set_protocol_event_handler(FIX_OBEX_EVENT(GOEP_ABORT_RSP), opp_msg_handler, 0);
	mmi_frm_set_protocol_event_handler(FIX_OBEX_EVENT(GOEP_DISCONNECT_IND), opp_msg_handler, 0);
	mmi_frm_set_protocol_event_handler(FIX_OBEX_EVENT(GOEP_AUTH_IND), opp_msg_handler, 0);
	mmi_frm_set_protocol_event_handler(FIX_OBEX_EVENT(GOEP_OPP_SUPPORTED_FORMATS_IND), opp_msg_handler, 0);

	buf_bt = vm_malloc(64 * 1024);
}

extern const unsigned char ProFont6x11[];

VMBOOL bt_opp_connect(VMUINT8* mac) {
	VMUINT8 mac8[8];
	bt_mac_conv(mac8, mac);
	print_hex("Mac:  ", mac, 6);
	print_hex("Mac8: ", mac8, 8);

	PLATFORM_ASSERT();
	
	/*conn_id = srv_bt_cm_start_conn(FALSE, 0xfffd, mac8, NULL);
	DEBUG_PRINTF("conn_id = %d\n", conn_id);
	if (conn_id < 0)
		return FALSE;*/

	srv_oppc_send_begin(opp_hndl, mac8, buf_bt, 64 * 1024);
	//oppc_send_connect_req(conn_id, buf_bt, 64 * 1024, mac8);
}

VMBOOL bt_opp_deinit() {
	PLATFORM_ASSERT();

	mmi_bt_obex_event_hdlr_init();

	//if (conn_id < 0)
	//	return FALSE;

	//srv_bt_cm_stop_conn(conn_id);
	if (opp_hndl < 0)
		return FALSE;

	int res = srv_opp_close(opp_hndl); 
	DEBUG_PRINTF("srv_opp_close ret %d\n", res);

	//return res;
}