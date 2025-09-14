#include <vmsys.h>
#include <vmio.h>
#include <vmgraph.h>
#include <vmchset.h>
#include <vmstdlib.h>
#include <vm4res.h>
#include <vmres.h>
#include <vmtimer.h>
#include <string.h>

#include <console.h>
#include <opp.h>
#include <IPtoStream.h>
#include <T2Input.h>
#include <UIEngine.h>
#include <Render.h>

VMINT		layer_hdl[2];	// layer handle array. 
VMUINT8* layer_buf[2] = {};

VMINT screen_w = 0;
VMINT screen_h = 0;

int tcp_id = -1;
int tcpl_id = -1;

bool connected = false;

void handle_sysevt(VMINT message, VMINT param); // system events 

T2Input t2input;
UIEngine uiengine;

VMUINT8 my_mac[6] = { 0x1C, 0xBF, 0xC0, 0x2A, 0xD8, 0xEA }; // temporarily here for testing
//VMUINT8 my_mac[6] = { 0x00, 0x1B, 0x10, 0x00, 0x2A, 0xEC }; // temporarily here for testing


extern "C" void flush_layer() {
	vm_graphic_flush_layer(layer_hdl, 2);
	vm_graphic_fill_rect(layer_buf[1], 0, 0, screen_w, screen_h, tr_color, tr_color);
}

void handle_keyevt(VMINT event, VMINT keycode) {
#ifdef WIN32   //Fix for MoDIS
	if (VM_KEY_NUM1 <= keycode && keycode <= VM_KEY_NUM3)
		keycode += 6;
	else if (VM_KEY_NUM7 <= keycode && keycode <= VM_KEY_NUM9)
		keycode -= 6;
#endif

	uiengine.KeyboardEvent(event, keycode);
	t2input.draw();
	flush_layer();
}

void text_event(const char* str) {
	uiengine.TextEvent(str);
	t2input.draw();
	flush_layer();
}

void handle_penevt(int event, int x, int y) {
	t2input.handle_penevt(event, x, y);
	t2input.draw();
	flush_layer();
}



extern "C" VMUINT8* get_buf() {
	return layer_buf[0];
}

void vm_main(void) {
	layer_hdl[0] = -1;
	screen_w = vm_graphic_get_screen_width();
	screen_h = vm_graphic_get_screen_height();

	layer_hdl[0] = vm_graphic_create_layer(0, 0, screen_w, screen_h, -1);
	layer_buf[0] = vm_graphic_get_layer_buffer(layer_hdl[0]);
	layer_hdl[1] = vm_graphic_create_layer(0, 0, screen_w, screen_h, tr_color);
	layer_buf[1] = vm_graphic_get_layer_buffer(layer_hdl[1]);
	vm_graphic_set_clip(0, 0, screen_w, screen_h);


	t2input.init();
	uiengine.init();

	uiengine.def_buf = (unsigned short*)layer_buf[0];
	t2input.scr_buf = (unsigned char*)layer_buf[1];
	t2input.layer_handle = layer_hdl[1];

	uiengine.PushUI(uise);

	vm_reg_sysevt_callback(handle_sysevt);
	vm_reg_keyboard_callback(handle_keyevt);
	vm_reg_pen_callback(handle_penevt);

	console_init(screen_w, screen_h, (VMUINT16*)layer_buf[0]);
	//cprintf("IPoverObexVxp Test injection\n");
#ifdef WIN32
	ipts.init(StreamType::TCP);
	ipts.connectTCP("127.0.0.1");
#else
	ipts.init(StreamType::BT);
	ipts.connectBT(my_mac);
#endif // WIN32

	uiengine.Draw((unsigned short*)layer_buf[0]);
	flush_layer();

	cprintf("IPoverObexVxp Test TCP\n");
}

void handle_sysevt(VMINT message, VMINT param) {
#ifdef		SUPPORT_BG
	switch (message) {
	case VM_MSG_CREATE:
		break;
	case VM_MSG_PAINT:
		break;
	case VM_MSG_HIDE:
		break;
	case VM_MSG_QUIT:
		ipts.quit();
		break;
	}
#else
	switch (message) {
	case VM_MSG_CREATE:
	case VM_MSG_ACTIVE:

		break;

	case VM_MSG_PAINT:
		break;

	case VM_MSG_INACTIVE:
		break;
	case VM_MSG_QUIT:
		ipts.quit();
		break;
	}
#endif
}



