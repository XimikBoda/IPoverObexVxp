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

VMINT		layer_hdl[1];	// layer handle array. 
VMUINT8* layer_buf = 0;

VMINT screen_w = 0;
VMINT screen_h = 0;

int tcp_id = -1;

void handle_sysevt(VMINT message, VMINT param); // system events 

VMUINT8 my_mac[6] = { 0x1C, 0xBF, 0xC0, 0x2A, 0xD8, 0xEA }; // temporarily here for testing

void key_handler(VMINT event, VMINT keycode) {
	bt_opp_flush();
	vm_graphic_flush_layer(layer_hdl, 1);
	if (event == VM_KEY_EVENT_UP) {
		if (keycode == VM_KEY_NUM1) {
			bt_opp_deinit();
			vm_exit_app();
		}
		else if (keycode == VM_KEY_NUM2) {
			bt_opp_disconnect();
		}
	}
}

extern "C" VMUINT8* get_buf() {
	return layer_buf;
}

void vm_main(void) {
	layer_hdl[0] = -1;
	screen_w = vm_graphic_get_screen_width();
	screen_h = vm_graphic_get_screen_height();

	layer_hdl[0] = vm_graphic_create_layer(0, 0, screen_w, screen_h, -1);
	layer_buf = vm_graphic_get_layer_buffer(layer_hdl[0]);
	vm_graphic_set_clip(0, 0, screen_w, screen_h);

	vm_reg_sysevt_callback(handle_sysevt);
	vm_reg_keyboard_callback(key_handler);

	console_init(screen_w, screen_h, (VMUINT16*)layer_buf);
	cprintf("IPoverObexVxp Test injection\n");

	bt_opp_preinit();
	bt_opp_init();
	bt_opp_connect(my_mac);

	cprintf("IPoverObexVxp Test TCP\n");

	tcp_id = ipts.tcp.connect("google.com", 80,
		[](int id, TCPEvent event) {
			const char* names[4] = {
				"Connected", "Disconnected",
				"HostNotFound", "Error",
			};

			cprintf("tcp_callback(%d, %d (%s))\n", id, event, names[event]);
		});

	const char* minimal_http = "POST /404 HTTP/1.0\r\n\r\n";
	ipts.tcp.write(tcp_id, minimal_http, strlen(minimal_http));

	vm_create_timer(100, [](int tid) {
		ipts.update();

		while (true) {
			char buf[101] = {};

			size_t size = ipts.tcp.read(tcp_id, buf, 100);

			if (size > 0) {
				buf[size] = 0;

				cprintf("%s", buf);
			}
			else
				break;
		}
		vm_graphic_flush_layer(layer_hdl, 1);
		});
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
		bt_opp_deinit();
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
		bt_opp_deinit();
		break;
	}
#endif
}

extern "C" void flush_layer() {
	vm_graphic_flush_layer(layer_hdl, 1);
}