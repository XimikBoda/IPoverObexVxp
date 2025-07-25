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
int tcpl_id = -1;

bool connected = false;

void handle_sysevt(VMINT message, VMINT param); // system events 

VMUINT8 my_mac[6] = { 0x1C, 0xBF, 0xC0, 0x2A, 0xD8, 0xEA }; // temporarily here for testing
//VMUINT8 my_mac[6] = { 0x00, 0x1B, 0x10, 0x00, 0x2A, 0xEC }; // temporarily here for testing

void key_handler(VMINT event, VMINT keycode) {
	//bt_opp_flush();
	vm_graphic_flush_layer(layer_hdl, 1);
	/*if (event == VM_KEY_EVENT_UP) {
		if (keycode == VM_KEY_NUM1) {
			bt_opp_deinit();
			vm_exit_app();
		}
		else if (keycode == VM_KEY_NUM2) {
			bt_opp_disconnect();
		}
	}*/
}

extern "C" VMUINT8* get_buf() {
	return layer_buf;
}

void tcp_callback(int id, TCPEvent event, uint32_t val) {
	const char* names[4] = {
			"Connected", "Disconnected",
			"HostNotFound", "Error",
	};

	cprintf("tcp_callback(%d, %d (%s), %#08x)\n", id, event, names[(int)event], val);
	switch (event)
	{
	case TCPEvent::Connected:
		connected = true;
		{
			const char* hello = "TCP Relay\n";
			ipts.tcp.write(tcp_id, hello, strlen(hello));
		}
		break;
	case TCPEvent::Disconnected:
	case TCPEvent::HostNotFound:
	case TCPEvent::Error:
		connected = false;
		ipts.tcp.close(tcp_id);
		tcp_id = ipts.tcp.init(tcp_callback);
		ipts.tcp.laccept(id, tcp_id);
		break;
	default:
		break;
	}
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
	//cprintf("IPoverObexVxp Test injection\n");
#ifdef WIN32
	ipts.init(StreamType::TCP);
	ipts.connectTCP("127.0.0.1");
#else
	ipts.init(StreamType::BT);
	ipts.connectBT(my_mac);
#endif // WIN32

	

	cprintf("IPoverObexVxp Test TCP\n");

	tcp_id = ipts.tcp.init(tcp_callback);

	tcpl_id = ipts.tcp.lbind(400, [](int id, TCPLEvent event) {
		const char* names[3] = {
				"Binded", "Accepted",
				"Error",
		};

		cprintf("tcpl_callback(%d, %d (%s))\n", id, event, names[(int)event]);

		switch (event) {
		case TCPLEvent::Binded:
			ipts.tcp.laccept(id, tcp_id);
			break;
		}
		});

	vm_create_timer(16, [](int tid) {
		ipts.update();

		while (connected) {
			char buf[101] = {};

			size_t size = ipts.tcp.read(tcp_id, buf, 100);

			if (size > 0) {
				buf[size] = 0;

				cprintf("%s", buf);

				ipts.tcp.write(tcp_id, buf, size);
				ipts.update();
			}
			else
				break;
		}
		vm_graphic_flush_layer(layer_hdl, 1);
		});

	vm_create_timer(2000, [](int tid) {
		ipts.log.post(Log::Level::Info, "App1", "Some simple text");

		ipts.log.pprintf(Log::Level::Debug, "App2", "Some printf like text, and ticks: %d", vm_get_tick_count());
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

extern "C" void flush_layer() {
	vm_graphic_flush_layer(layer_hdl, 1);
}