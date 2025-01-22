#include <vmsys.h>
#include <console.h>
#include <string.h>
#include "opp.h"

static VMUINT32 bt_obex_events_base = 0;

static VMUINT32 find_a2(const VMUINT8* find_buf, int len) {
	for (int i = 0x700000; i < 0x1000000; i += 2) {
		VMUINT8* adr = (VMUINT8*)(((VMUINT32)vm_get_sym_entry) & (0xFF000000)) + i;
		if (!memcmp(adr, find_buf, len)) {
			return (((VMUINT32)adr) | 1);
			break;
		}
	}
	return 0;
}

static VMUINT32 find_a4(const VMUINT8* find_buf, int len) {
	for (int i = 0x700000; i < 0x1000000; i += 4) {
		VMUINT8* adr = (VMUINT8*)(((VMUINT32)vm_get_sym_entry) & (0xFF000000)) + i;
		if (!memcmp(adr, find_buf, len)) {
			return (((VMUINT32)adr) | 1);
			break;
		}
	}
	return 0;
}

static VMUINT32 find2_a4(const VMUINT8* find_buf1, const VMUINT8* find_buf2, int len) {
	for (int i = 0x700000; i < 0x1000000; i += 4) {
		VMUINT8* adr = (VMUINT8*)(((VMUINT32)vm_get_sym_entry) & (0xFF000000)) + i;

		int j = 0;
		for (; j < len; ++j)
			if (find_buf1[j] == find_buf2[j] && adr[j] != find_buf1[j])
				break;

		if (j == len) {
			return (((VMUINT32)adr) | 1);
			break;
		}
	}
	return 0;
}

static VMUINT32 thumb_bl(void* adr) {
	VMUINT16 high = ((VMUINT16*)adr)[0];
	VMUINT16 low = ((VMUINT16*)adr)[1];

	VMUINT16 imm10 = high & 0x03FF;
	VMUINT16 imm11 = low & 0x07FF;

	VMUINT8 J1 = (low >> 13) & 1;
	VMUINT8 J2 = (low >> 11) & 1;
	VMUINT8 S = (high >> 10) & 1;

	int I1 = !(J1 ^ S);
	int I2 = !(J2 ^ S);

	VMUINT32 offset = (I1 << 23) | (I2 << 22) | (imm10 << 12) | (imm11 << 1);
	if (S)
		offset |= (0xFF << 24);

	return offset + (VMUINT32)adr + 4;
}

void (*mmi_bt_obex_event_hdlr_init)(void) = 0;
void (*mmi_frm_set_protocol_event_handler)(VMUINT16 eventID, MSGHandler funcPtr, VMBOOL isMultiHandler) = 0;

void (*srv_bt_cm_connect_ind)(VMUINT32 conn_id) = 0;
void (*srv_bt_cm_stop_conn)(VMUINT32 conn_id) = 0;

VMUINT32 (*srv_opp_open)(VMUINT8 role) = 0;
VMUINT32 (*srv_opp_close)(VMUINT32 srv_hd) = 0;

void (*srv_oppc_send_push_req)(int goep_conn_id, int pkt_type, VMINT32 total_obj_len, VMWSTR obj_name, int obj_mime, VMUINT32* frag_ptr, VMUINT16 frag_len) = 0;
void (*srv_oppc_send_abort_req)(VMINT8 goep_conn_id) = 0;
void (*srv_oppc_send_disconnect_req)(VMINT8 goep_conn_id, int tpdisconn_flag) = 0;
VMINT32 (*srv_oppc_send_begin)(VMINT32 srv_hd, void* dst_dev, VMUINT8* buffer, VMUINT16 buf_size) = 0;
void (*srv_oppc_notify_app)(VMINT32 event_id, void* para) = 0;

// mmi
static const VMUINT8 mmi_bt_obex_event_hdlr_init_Magic[] = { 0x10, 0xB5, 0x2D, 0x49, 0x2D, 0x48, 0x00, 0x22 };
//static const VMUINT8 mmi_frm_set_protocol_event_handler_Magic[] = { 0x13, 0x00, 0x0A, 0x00, 0x01, 0x00, 0x00, 0xB5, 0x00, 0x20 };

// srv_bt
static const VMUINT8 srv_bt_cm_connect_ind_Magic_1[] = { 0xF0, 0xB5, 0x04, 0x00, 0x85, 0xB0, 0x3F, 0xD0, 0x06, 0x2C, 0x3D, 0xD8, 0x20, 0x00 }; // I hope this works.
static const VMUINT8 srv_bt_cm_connect_ind_Magic_2[] = { 0x70, 0xB5, 0x04, 0x00, 0x86, 0xB0, 0x52, 0xD0, 0x06, 0x2C, 0x50, 0xD8, 0x20, 0x00 };

static const VMUINT8 srv_bt_cm_stop_conn_Magic_before_bl[] = { 0x60, 0x68, 0x03, 0x21, 0x41, 0x73, 0x42, 0x6A, 0x01, 0x6A, 0x02, 0x92, 0x01, 0x91, 0x80, 0x69 };

// srv_opp
static const VMUINT8 srv_opp_open_Magic[] = { 0x09, 0x49, 0x01, 0x28, 0x05, 0xD1, 0x09, 0x68, 0x08, 0x68, 0x00, 0x28, 0x09, 0xD1, 0x01, 0x20, 0x05, 0xE0 };
static const VMUINT8 srv_opp_close_Magic[] = { 0x70, 0xB5, 0x01, 0x21, 0x09, 0x4C, 0x49, 0x02, 0x00, 0x25, 0x88, 0x42, 0x01, 0xDB, 0x60, 0x68, 0x08, 0xE0 };

// srv_oppc
static const VMUINT8 srv_oppc_send_push_req_Magic[] = { 0xFF, 0xB5, 0x06, 0x00, 0x00, 0x22, 0x0F, 0x00, 0x1D, 0x00, 0x99, 0x20, 0x11, 0x00, 0x80, 0x00, 0x13, 0x00, 0x81, 0xB0 };
static const VMUINT8 srv_oppc_send_abort_req_Magic_1[] = { 0x10, 0xB5, 0x00, 0x22, 0x04, 0x00, 0x11, 0x00, 0x06, 0x20, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x71, 0x01, 0x48 };
static const VMUINT8 srv_oppc_send_abort_req_Magic_2[] = { 0x10, 0xB5, 0x00, 0x22, 0x04, 0x00, 0x11, 0x00, 0x06, 0x20, 0x13, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x04, 0x71, 0x01, 0x48 };
static const VMUINT8 srv_oppc_send_disconnect_req_Magic_1[] = { 0x70, 0xB5, 0x00, 0x22, 0x04, 0x00, 0x0D, 0x00, 0x11, 0x00, 0x06, 0x20, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x49, 0x44, 0x71, 0x09, 0x68 };
static const VMUINT8 srv_oppc_send_disconnect_req_Magic_2[] = { 0x70, 0xB5, 0x00, 0x22, 0x04, 0x00, 0x0D, 0x00, 0x11, 0x00, 0x06, 0x20, 0x13, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x08, 0x49, 0x44, 0x71, 0x09, 0x68 };
static const VMUINT8 srv_oppc_send_begin_Magic[] = { 0xFF, 0xB5, 0x14, 0x48, 0x02, 0x27, 0x04, 0x68, 0x0D, 0x00, 0x0C, 0x34, 0x60, 0x78, 0x16, 0x00, 0xFF, 0x43, 0x00, 0x28, 0x81, 0xB0, 0x0D, 0xD1 };
static const VMUINT8 srv_oppc_notify_app_Magic[] = { 0x30, 0xB4, 0x04, 0x00, 0x07, 0x48, 0x03, 0x68, 0x18, 0x68, 0x00, 0x28, 0x07, 0xDD, 0x5A, 0x68, 0x22, 0x42, 0x04, 0xD0, 0x0A, 0x00, 0x9B, 0x68 };


#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

#define DEBUG_PRINT_INJECT(x) DEBUG_PRINTF(#x "->0x%08X\n", x);

#define INJECT_ASSERT(x) if (!x) return FALSE;

#define Magic_TO_BS(x) (x), sizeof(x)
#define Magic_TO_BS2(x) (x##_1), (x##_2), sizeof(x##_1)

#define INJECT_NE(x) \
	x = (void*)find_a4(Magic_TO_BS(x##_Magic)); \
	DEBUG_PRINT_INJECT(x);

#define INJECT_NE2(x) \
	x = (void*)find2_a4(Magic_TO_BS2(x##_Magic)); \
	DEBUG_PRINT_INJECT(x);

#define INJECT_NEbBL(x) \
	VMUINT32 x##_before = find_a2(Magic_TO_BS(x##_Magic_before_bl)); \
	DEBUG_PRINT_INJECT(x##_before); INJECT_ASSERT(x##_before); \
	x = (void*)thumb_bl((void*)(x##_before + sizeof(x##_Magic_before_bl)));\
	DEBUG_PRINT_INJECT(x);\
	

#define INJECT(x) INJECT_NE(x) INJECT_ASSERT(x)
#define INJECT2(x) INJECT_NE2(x) INJECT_ASSERT(x)
#define INJECTbBL(x) INJECT_NEbBL(x) INJECT_ASSERT(x)

VMBOOL bt_opp_preinit() {
	PLATFORM_ASSERT();

	INJECT(mmi_bt_obex_event_hdlr_init);
	//INJECT(mmi_frm_set_protocol_event_handler);

	mmi_frm_set_protocol_event_handler = (void*)thumb_bl((char*)mmi_bt_obex_event_hdlr_init + 8);
	DEBUG_PRINT_INJECT(mmi_frm_set_protocol_event_handler);

	INJECT2(srv_bt_cm_connect_ind);
	INJECTbBL(srv_bt_cm_stop_conn);

	INJECT(srv_opp_open);
	INJECT(srv_opp_close);

	INJECT(srv_oppc_send_push_req);
	INJECT2(srv_oppc_send_abort_req);
	INJECT2(srv_oppc_send_disconnect_req);
	INJECT(srv_oppc_send_begin);
	INJECT(srv_oppc_notify_app);

	bt_obex_events_base = *(VMUINT32*)((((VMUINT32)mmi_bt_obex_event_hdlr_init) & ~1uL) + 0xbc);
	DEBUG_PRINTF("bt_obex_events_base->%d\n", bt_obex_events_base);

	return TRUE;
}