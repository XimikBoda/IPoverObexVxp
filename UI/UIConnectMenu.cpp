#include "UIConnectMenu.h"
#include "Render.h"
#include "UIEngine.h"
#include "vmstdlib.h"
#include "vmio.h"
#include "string.h"
#include <vmbtcm.h>

extern UIEngine uiengine;

void UIConnectMenu::Draw(unsigned short* buf)
{
	Render::draw_fill_rect(buf, 0, 0, screen_w, screen_h - 110, 0x0000);

	int y = 3;
	int x = 5;

	Render::draw_text_white_centered(buf, 0, y, screen_w, "IPoverObexVxp");

	Render::draw_fill_rect(buf, 0, y + 13, screen_w, 1, 0xFFFF);

	y += 20;
	Render::draw_text_white(buf, 5, y, "BT Mac to connect:");
	y += 15;

	macImput.y = y;

	y += 20;
	Render::draw_text_white(buf, 5, y, "Paired devices:");
	y += 15;

	pairedDevices.y = y;

	y += 20;
	connectButton.y = y;

	for (int i = 0; i < uiarray_size_internal; ++i)
		uiarray[i].el->QDraw(buf);
}

void UIConnectMenu::RightSoftkeyClick() { 
	uiengine.PopUI(); 
}

void UIConnectMenu::ClickConnect()
{

}

int UIConnectMenu::PairedGetLen() {
	return 1 + vm_btcm_get_dev_num(VM_SRV_BT_CM_PAIRED_DEV);
}

static void bt_mac_conv(VMUINT8* in, VMUINT8* out) {
	out[5] = in[0];
	out[4] = in[1];
	out[3] = in[2];
	out[2] = in[4];
	out[1] = in[6];
	out[0] = in[7];
}

static const char* get_bt_mac_str(vm_srv_bt_cm_bt_addr bd_addr) {
	unsigned char mac6[6] = {};
	bt_mac_conv((VMUINT8*)&bd_addr, mac6);

	static char str[20] = {};

	for (int i = 0; i < 6; ++i)
		sprintf(str + i * 3, "%02x:", mac6[i]);

	str[3 * 6 - 1] = 0;
	return str;
}

const char* UIConnectMenu::PairedGetElement(int id)
{
	if (id == 0)
		return "-Select paired device-";
	
	--id;

	vm_srv_bt_cm_dev_struct info = {};
	int res = vm_btcm_get_dev_info_by_index(id, VM_SRV_BT_CM_PAIRED_DEV, &info);
	if (res < 0)
		return "-Error-";

	static char str[200] = "";
	sprintf(str, "%s (%s)", info.name, get_bt_mac_str(info.bd_addr));

	return str;
}
