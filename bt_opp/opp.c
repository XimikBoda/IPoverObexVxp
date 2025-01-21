#include <vmsys.h>
#include <console.h>
#include <string.h>
#include "opp.h"

static VMUINT32 bt_obex_events_base = 0;

static VMUINT32 find_a4(const VMUINT8* find_buf, int len) {
	for (int i = 0; i < 0x1000000; i += 4) {
		VMUINT8* adr = (VMUINT8*)(((VMUINT32)vm_get_sym_entry) & (0xFF000000)) + i;
		if (!memcmp(adr, find_buf, len)) {
			return (((VMUINT32)adr) | 1);
			break;
		}
	}
	return 0;
}

static VMUINT32 find2_a4(const VMUINT8* find_buf1, const VMUINT8* find_buf2, int len) {
	for (int i = 0; i < 0x1000000; i += 4) {
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



struct magik_el{
	const VMUINT8* magic_buf;
	const VMUINT32 size;
};


typedef VMUINT8(*MSGHandler) (void* local_buf, int src_mod, ilm_struct* ilm);

void (*mmi_bt_obex_event_hdlr_init)(void) = 0;
void (*mmi_frm_set_protocol_event_handler)(VMUINT16 eventID, MSGHandler funcPtr, VMBOOL isMultiHandler) = 0;

void (*srv_bt_cm_connect_ind)(VMUINT32 conn_id) = 0;
void (*srv_bt_cm_stop_conn)(VMUINT32 conn_id) = 0;

#define MAGIK_TO_BS(x) (x), sizeof(x)
#define MAGIK_TO_BS2(x) (x##_1), (x##_2), sizeof(x##_1)

static const VMUINT8 mmi_bt_obex_event_hdlr_init_Magik[] = { 0x10, 0xB5, 0x2D, 0x49, 0x2D, 0x48, 0x00, 0x22 };
static const VMUINT8 mmi_frm_set_protocol_event_handler_Magik[] = { 0x13, 0x00, 0x0A, 0x00, 0x01, 0x00, 0x00, 0xB5, 0x00, 0x20 };

static const VMUINT8 srv_bt_cm_connect_ind_Magik_1[] = { 0xF0, 0xB5, 0x04, 0x00, 0x85, 0xB0, 0x3F, 0xD0, 0x06, 0x2C, 0x3D, 0xD8, 0x20, 0x00 };
static const VMUINT8 srv_bt_cm_connect_ind_Magik_2[] = { 0x70, 0xB5, 0x04, 0x00, 0x86, 0xB0, 0x52, 0xD0, 0x06, 0x2C, 0x50, 0xD8, 0x20, 0x00 };

static const VMUINT8 srv_bt_cm_stop_conn_Magik_1[] = { 0xF8, 0xB5, 0x04, 0x00, 0x7A, 0xD0, 0x06, 0x2C, 0x78, 0xD8, 0x00, 0x22, 0x11, 0x00, 0x14, 0x20 };
static const VMUINT8 srv_bt_cm_stop_conn_Magik_2[] = { 0xFE, 0xB5, 0x04, 0x00, 0x7E, 0xD0, 0x06, 0x2C, 0x7C, 0xD8, 0x78, 0x4D, 0x20, 0x01, 0x41, 0x19 };
static const VMUINT8 srv_bt_cm_stop_conn_Magik_3[] = { 0xF0, 0xB5, 0x05, 0x00, 0x85, 0xB0, 0x7E, 0xD0, 0x06, 0x2D, 0x7C, 0xD8, 0x7C, 0x48, 0x29, 0x01, 0x09, 0x18 };

static const struct magik_el srv_bt_cm_stop_conn_Magik_Arr[] = {
	{MAGIK_TO_BS(srv_bt_cm_stop_conn_Magik_1)},
	{MAGIK_TO_BS(srv_bt_cm_stop_conn_Magik_2)},
	{MAGIK_TO_BS(srv_bt_cm_stop_conn_Magik_3)},
};

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

#define DEBUG_PRINTF(...) cprintf(__VA_ARGS__) //TODO

#define DEBUG_PRINT_INJECT(x) DEBUG_PRINTF(#x "->0x%08X\n", x);

#define INJECT_NE(x) \
	x = (void*)find_a4(MAGIK_TO_BS(x##_Magik)); \
	DEBUG_PRINT_INJECT(x);

#define INJECT_NE2(x) \
	x = (void*)find2_a4(MAGIK_TO_BS2(x##_Magik)); \
	DEBUG_PRINT_INJECT(x);

#define INJECT_NE_ARR(x, i) \
	x = (void*)find_a4(x##_Magik_Arr[i].magic_buf, x##_Magik_Arr[i].size); 

#define INJECT(x) INJECT_NE(x) if (!x) return FALSE;
#define INJECT2(x) INJECT_NE2(x) if (!x) return FALSE;
#define INJECT_ARR(x) \
	for(int i = 0; i < NELEMS(x##_Magik_Arr) && !(x); ++i) { INJECT_NE_ARR(x, i) } \
	DEBUG_PRINT_INJECT(x);  if (!x) return FALSE;


VMBOOL bt_opp_pre_init() {
#ifdef WIN32
	DEBUG_PRINTF("WIN32 is not support for now\n");
	return FALSE;
#endif // WIN32

	INJECT(mmi_bt_obex_event_hdlr_init);
	INJECT(mmi_frm_set_protocol_event_handler);

	INJECT2(srv_bt_cm_connect_ind);
	INJECT_ARR(srv_bt_cm_stop_conn);

	bt_obex_events_base = *(VMUINT32*)((((VMUINT32)mmi_bt_obex_event_hdlr_init) & ~1uL) + 0xbc);
	DEBUG_PRINTF("bt_obex_events_base->%d\n", bt_obex_events_base);
}