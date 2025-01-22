#include <vmsys.h>
#include <vmbtcm.h>
#include <console.h>
#include <string.h>
#include "opp.h"


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


VMBOOL bt_opp_init() {
	PLATFORM_ASSERT();

	DEBUG_PRINTF("BT statud: %s\n", bt_pw_st[vm_btcm_get_power_status()]);

	opp_hndl = srv_opp_open(OPP_ROLE_CLIENT);
	DEBUG_PRINTF("opp_hndl = %d\n", opp_hndl);
	if (opp_hndl < 0)
		return FALSE;
}

VMBOOL bt_opp_connect(VMUINT8* mac) {
	VMUINT8 mac8[8];
	bt_mac_conv(mac8, mac);
	print_hex("Mac:  ", mac, 6);
	print_hex("Mac8: ", mac8, 8);

	PLATFORM_ASSERT();
}

VMBOOL bt_opp_deinit() {
	PLATFORM_ASSERT();

	mmi_bt_obex_event_hdlr_init();

	if (opp_hndl < 0)
		return FALSE;
	int res = srv_opp_close(opp_hndl); 
	DEBUG_PRINTF("srv_opp_close ret %d\n", res);

	return res;
}