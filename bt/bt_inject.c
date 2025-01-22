#include <vmsys.h>
#include <console.h>
#include <string.h>
#include "bt.h"

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


// mmi
static const VMUINT8 mmi_bt_obex_event_hdlr_init_Magic[] = { 0x10, 0xB5, 0x2D, 0x49, 0x2D, 0x48, 0x00, 0x22 };
//static const VMUINT8 mmi_frm_set_protocol_event_handler_Magic[] = { 0x13, 0x00, 0x0A, 0x00, 0x01, 0x00, 0x00, 0xB5, 0x00, 0x20 };

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

VMBOOL bt_preinit() {
	PLATFORM_ASSERT();

	INJECT(mmi_bt_obex_event_hdlr_init);
	//INJECT(mmi_frm_set_protocol_event_handler);

	mmi_frm_set_protocol_event_handler = (void*)thumb_bl((char*)mmi_bt_obex_event_hdlr_init + 8);
	DEBUG_PRINT_INJECT(mmi_frm_set_protocol_event_handler);


	bt_obex_events_base = *(VMUINT32*)((((VMUINT32)mmi_bt_obex_event_hdlr_init) & ~1uL) + 0xbc);
	DEBUG_PRINTF("bt_obex_events_base->%d\n", bt_obex_events_base);

	return TRUE;
}