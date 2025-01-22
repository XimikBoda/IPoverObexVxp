#include <vmsys.h>
#include <vmbtcm.h>
#include <console.h>
#include <string.h>
#include "bt.h"

extern VMUINT32 bt_spp_events_base;
#define FIX_SPP_EV(x) ((x) + bt_spp_events_base)

extern VMUINT32 bt_dest_module;
extern VMUINT32 bt_sap_id;

static VMINT32 opp_hndl = -1;

static const char* bt_pw_st[4] = {
	"On", "Off", "Switching on", "Switching off"
};

void bt_mac_conv(VMUINT8* out, VMUINT8* in) {
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

static VMUINT8 bt_spp_event_handler(void* msg, int event_id) {
	DEBUG_PRINTF("bt_spp_msg_event(%d)\n", event_id);
}

static VMUINT8 bt_spp_msg_handler(void* msg, int src_mod, ilm_struct* ilm) {
	bt_spp_event_handler(msg, ilm->msg_id);
}


VMBOOL bt_spp_init() {
	PLATFORM_ASSERT();

	DEBUG_PRINTF("BT statud: %s\n", bt_pw_st[vm_btcm_get_power_status()]);

	mmi_frm_set_protocol_event_handler(FIX_SPP_EV(BT_SPP_CONNECT_IND_REQ), bt_spp_msg_handler, 0);
	mmi_frm_set_protocol_event_handler(FIX_SPP_EV(BT_SPP_CONNECT_IND), bt_spp_msg_handler, 0);
	mmi_frm_set_protocol_event_handler(FIX_SPP_EV(BT_SPP_CONNECT_CNF), bt_spp_msg_handler, 0);
	mmi_frm_set_protocol_event_handler(FIX_SPP_EV(BT_SPP_ACTIVATE_CNF), bt_spp_msg_handler, 0);
	mmi_frm_set_protocol_event_handler(FIX_SPP_EV(BT_SPP_DEACTIVATE_CNF), bt_spp_msg_handler, 0);
	mmi_frm_set_protocol_event_handler(FIX_SPP_EV(BT_SPP_DISCONNECT_CNF), bt_spp_msg_handler, 0);
	mmi_frm_set_protocol_event_handler(FIX_SPP_EV(BT_SPP_DISCONNECT_IND), bt_spp_msg_handler, 0);
	mmi_frm_set_protocol_event_handler(FIX_SPP_EV(BT_SPP_UART_OWNER_IND), bt_spp_msg_handler, 0);
}

int conn_id = -1;

VMUINT8 tx_buf[10 * 1024];
VMUINT8 rx_buf[10 * 1024];

static void spp_connect_req(vm_srv_bt_cm_bt_addr* addr) {
	bt_spp_connect_req_struct* conn_req;

	DEBUG_PRINTF("Before construct_local_para\n");
	conn_req = (bt_spp_connect_req_struct*)construct_local_para(sizeof(*conn_req), 0);

	conn_req->lap = addr->lap;
	conn_req->uap = addr->uap;
	conn_req->nap = addr->nap;

	conn_req->txBufPtr = tx_buf;
	conn_req->txBufSize = 10 * 1024;
	conn_req->rxBufPtr = rx_buf;
	conn_req->rxBufSize = 10 * 1024;

	conn_req->uuid = 0x1101;
	conn_req->sec_level = 0xFF;
	conn_req->req_id = 0x1101;

	DEBUG_PRINTF("Before srv_opp_send_ilm\n");
	srv_opp_send_ilm(FIX_SPP_EV(BT_SPP_CONNECT_REQ), (void*)conn_req);
	DEBUG_PRINTF("After srv_opp_send_ilm\n");
}

VMBOOL bt_spp_connect(VMUINT8* mac) {
	VMUINT8 mac8[8];
	bt_mac_conv(mac8, mac);
	print_hex("Mac:  ", mac, 6);
	print_hex("Mac8: ", mac8, 8);

	PLATFORM_ASSERT();

	conn_id = srv_bt_cm_start_conn(FALSE, 0x1101, mac8, NULL);
	DEBUG_PRINTF("conn_id: %d\n", conn_id);

	spp_connect_req((vm_srv_bt_cm_bt_addr*)mac8);
}

VMBOOL bt_deinit() {
	PLATFORM_ASSERT();

	srv_bt_cm_stop_conn(conn_id);
}
