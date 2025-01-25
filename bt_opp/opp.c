#include <vmsys.h>
#include <vmbtcm.h>
#include <vmchset.h>
#include <console.h>
#include <string.h>
#include "opp.h"

extern VMUINT32 bt_obex_events_base;
#define FIX_OBEX_EVENT(x) ((x) + bt_obex_events_base)

static VMINT32 connc_id = -1;
static VMINT32 conns_id = -1;

static VMINT32 obexc_id = -1;
static VMINT32 obexs_id = -1;


static VMINT32 opc_mtu = 0;

static void* buf_bt = 0;



static const char* bt_pw_st[4] = {
	"On", "Off", "Switching on", "Switching off"
};

static const char* bt_obex_events_names[40] = {
	"GOEP_REGISTER_SERVER_REQ",
	"GOEP_REGISTER_SERVER_RSP",
	"GOEP_DEREGISTER_SERVER_REQ",
	"GOEP_DEREGISTER_SERVER_RSP",
	"GOEP_CONNECT_IND",
	"GOEP_CONNECT_RES",
	"GOEP_PUSH_IND",
	"GOEP_PUSH_RES",
	"GOEP_PULL_IND",
	"GOEP_PULL_RES",
	"GOEP_SET_FOLDER_IND",
	"GOEP_SET_FOLDER_RES",
	"GOEP_ABORT_IND",
	"GOEP_ABORT_RES",
	"GOEP_AUTH_REQ",
	"GOEP_AUTH_RSP",
	"GOEP_REGISTER_CLIENT_REQ",
	"GOEP_REGISTER_CLIENT_RSP",
	"GOEP_DEREGISTER_CLIENT_REQ",
	"GOEP_DEREGISTER_CLIENT_RSP",
	"GOEP_CONNECT_REQ",
	"GOEP_CONNECT_RSP",
	"GOEP_PUSH_REQ",
	"GOEP_PUSH_RSP",
	"GOEP_PULL_REQ",
	"GOEP_PULL_RSP",
	"GOEP_SET_FOLDER_REQ",
	"GOEP_SET_FOLDER_RSP",
	"GOEP_ABORT_REQ",
	"GOEP_ABORT_RSP",
	"GOEP_DISCONNECT_REQ",
	"GOEP_DISCONNECT_RSP",
	"GOEP_DISCONNECT_IND",
	"GOEP_DISCONNECT_RES",
	"GOEP_TPDISCONNECT_REQ",
	"GOEP_AUTH_IND",
	"GOEP_AUTH_RES",
	"GOEP_OPP_SUPPORTED_FORMATS_IND",
	"GOEP_AUTHORIZE_IND",
	"GOEP_AUTHORIZE_RES",
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

static VMWSTR name[128];
static VMSTR mime[80] = { 0 };
static void oppc_connect_rsp_handler(void* msg) {
	goep_connect_rsp_struct* rsp = (goep_connect_rsp_struct*)msg;
	vm_ascii_to_ucs2(name, 128*2, "IpOverObex.txt");

	if (rsp->rsp_code == GOEP_STATUS_SUCCESS)
	{
		if (rsp->peer_mru != 0 && rsp->peer_mru < MAX_OBEX_PACKET_LENGTH)
			opc_mtu = rsp->peer_mru;
		else
			opc_mtu = MAX_OBEX_PACKET_LENGTH;

		//srv_bt_cm_connect_ind(rsp->req_id);

		srv_oppc_send_push_req(rsp->req_id, GOEP_FIRST_PKT, 0x7FFFFFFF, name + 1, mime, 0, 0);
	}
	else
	{
		//srv_bt_cm_stop_conn(rsp->req_id);
	}
}

static void oppc_push_rsp_handler(void* msg) {
	goep_push_rsp_struct* rsp = (goep_push_rsp_struct*)msg;

	if (rsp->rsp_code != GOEP_STATUS_SUCCESS)
	{
		srv_oppc_send_disconnect_req(rsp->goep_conn_id, TRUE);
		return;
	}

	obexc_id = rsp->goep_conn_id;
	//srv_oppc_send_push_req(rsp->goep_conn_id, GOEP_NORMAL_PKT, 0x7FFFFFFF, name, mime, get_buf(), opc_mtu);
}

static void opps_general_rsp(VMUINT32 msg_id, VMUINT8 conn_id, VMUINT8 rsp_code) {
	goep_res_struct* res;
	res = (goep_res_struct*)construct_local_para(sizeof(*res), 0);

	res->goep_conn_id = conn_id;
	res->rsp_code = rsp_code;

	srv_opp_send_ilm(FIX_OBEX_EVENT(msg_id), res);
}

static void opps_authorize_ind_hdler(void* msg) {
	goep_authorize_ind_struct* ind = (goep_authorize_ind_struct*)msg;

	obexs_id = ind->goep_conn_id;

	//conns_id = srv_bt_cm_start_conn(TRUE, 0x1105, &(ind->bd_addr), (VMINT8*)ind->dev_name);

	DEBUG_PRINTF("opps_authorize_ind_hdler(obexs_id = %d)\n", obexs_id);

	//if (conns_id < 0)
	//{
	//	opps_authorize_rsp(ind->goep_conn_id, GOEP_SERVICE_UNAVAILABLE);
	//	return;
	//}else
	opps_general_rsp(GOEP_AUTHORIZE_RES, ind->goep_conn_id, GOEP_STATUS_SUCCESS);


}

static void opps_connect_ind_handler(void* msg)
{
	goep_connect_ind_struct* ind = (goep_connect_ind_struct*)msg;

	DEBUG_PRINTF("opps_authorize_ind_hdler(obexs_id = %d)\n", ind->goep_conn_id);

	opps_general_rsp(GOEP_CONNECT_RES, ind->goep_conn_id, GOEP_STATUS_SUCCESS);

	//srv_bt_cm_connect_ind(conns_id);
}

static VMUINT8 opp_event_handler(int msg_id, void* msg) {
	DEBUG_PRINTF("event(%d, %s)\n", msg_id, bt_obex_events_names[msg_id - bt_obex_events_base]);
	switch (msg_id - bt_obex_events_base) {
	case GOEP_CONNECT_RSP:
		oppc_connect_rsp_handler(msg);
		break;
	case GOEP_PUSH_RSP:
		oppc_push_rsp_handler(msg);
		break;
	case GOEP_AUTHORIZE_IND:
		opps_authorize_ind_hdler(msg);
		break;
	case GOEP_CONNECT_IND:
		opps_connect_ind_handler(msg);
		break;
	}
}

static VMUINT8 opp_msg_handler(void* msg, int src_mod, ilm_struct* ilm) {
	opp_event_handler(ilm->msg_id, msg);
}

static void oppc_send_connect_req(VMINT conn_id, VMUINT8 buf, VMUINT16 buf_size, vm_srv_bt_cm_bt_addr* addr) {
	goep_connect_req_struct* req;

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

	srv_opp_send_ilm(FIX_OBEX_EVENT(GOEP_CONNECT_REQ), req);
}


VMBOOL bt_opp_init() {
	PLATFORM_ASSERT();

	DEBUG_PRINTF("BT statud: %s\n", bt_pw_st[vm_btcm_get_power_status()]);

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

	//connc_id = srv_bt_cm_start_conn(FALSE, 0xfffd, mac8, NULL);
	//DEBUG_PRINTF("connc_id = %d\n", connc_id);
	//if (connc_id < 0)
	//	return FALSE;

	//oppc_send_connect_req(connc_id, buf_bt, 64 * 1024, mac8);
}

VMBOOL bt_opp_deinit() {
	PLATFORM_ASSERT();

	mmi_bt_obex_event_hdlr_init();

	//if (connc_id < 0)
	//	return FALSE;

	//srv_bt_cm_stop_conn(connc_id);
}