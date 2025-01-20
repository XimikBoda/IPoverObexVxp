#include <vmsys.h>
#include <console.h>
#include "opp.h"

static VMUINT32 bt_obex_events_base = 0;

static VMUINT32 find_a4(const VMUINT8* find_buf, int len) {
	for (int i = 0; i < 0x1000000; i += 4) {
		VMUINT8* adr = (VMUINT8*)(((VMUINT32)vm_get_sym_entry) & (0xFF000000)) + i;
		int j = 0;
		for (; j < len; ++j)
			if (adr[j] != find_buf[j])
				break;

		if (j == len) {
			return (((VMUINT32)adr) | 1);
			break;
		}
	}
	return 0;
}

void (*mmi_bt_obex_event_hdlr_init)(void) = 0;

#define MAGIK_TO_BS(x) (x), sizeof(x)
static const VMUINT8 mmi_bt_obex_event_hdlr_init_magik[] = { 0x10, 0xB5, 0x2D, 0x49, 0x2D, 0x48, 0x00, 0x22 };

#define DEBUG_PRINTF(...) cprintf(__VA_ARGS__) //TODO

#define INJECT(x) \
	x = (void*)find_a4(MAGIK_TO_BS(x##_magik)); \
	DEBUG_PRINTF(#x "->0x%08X\n", x); if (!x) return FALSE;


VMBOOL bt_opp_pre_init() {
#ifdef WIN32
	DEBUG_PRINTF("WIN32 is not support for now\n");
	return FALSE;
#endif // WIN32

	INJECT(mmi_bt_obex_event_hdlr_init);

	bt_obex_events_base = *(VMUINT32*)((((VMUINT32)mmi_bt_obex_event_hdlr_init) & ~1uL) + 0xbc);
	DEBUG_PRINTF("bt_obex_events_base->%d\n", bt_obex_events_base);

}