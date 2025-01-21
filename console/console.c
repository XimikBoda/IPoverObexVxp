#include "console.h"
#include "Profont6x11.h"

#include <vmgraph.h>

const int char_width = 6;
const int char_height = 11;

static int scr_width = 0;
static int scr_height = 0;

static int c_w = 0;
static int c_x = 0;

static char need_scroll = 0;

static VMUINT16* scr_buf = 0;

int console_get_c_w() {
	return c_w;
}

void console_init(int _scr_w, int _scr_h, unsigned short* _scr_buf) {
	scr_width = _scr_w;
	scr_height = _scr_h;
	scr_buf = _scr_buf;
	c_w = scr_width / char_width;
}

static void scroll_up() {
	int line_size = scr_width * char_height;
	memmove(scr_buf, scr_buf + line_size, (scr_width * scr_height - line_size) * 2);
	memset(scr_buf + (scr_width * scr_height - line_size), 0, line_size * 2);
}

static void draw_xy_char(int x, int y, char c) {
	const unsigned char* font_ch = ProFont6x11 + 5 + 12 * c + 1;
	const unsigned short textcolor = 0xFFFF, backcolor = 0;

	for (int i = 0; i < char_height; ++i) {
		unsigned short* scr_buf_line = scr_buf + x + (y + i) * scr_width;
		for (int j = 0; j < char_width; ++j)
			scr_buf_line[j] = ((((*font_ch) >> j) & 1) ? textcolor : backcolor);
		++font_ch;
	}
}

void console_put_char(char c) {
	if (need_scroll || (c_x == c_w && c != '\n')) {
		scroll_up();
		need_scroll = 0;
		c_x = 0;
	}
	if (c == '\n') {
		need_scroll = 1;
		flush_layer();
		return;
	}
	draw_xy_char(c_x * char_width, scr_height - char_height, c);
	++c_x;
}

void console_put_str(const char* str) {
	while (*str) {
		console_put_char(*str);
		++str;
	}
	//flush_layer();
}

static char buf[1024];

#include <stdarg.h>
#include <vmstdlib.h>

int cprintf(char const* const format, ...) {
	va_list aptr;

	va_start(aptr, format);
	int ret = vm_vsprintf(buf, format, aptr);
	va_end(aptr);

	console_put_str(buf);
	return ret;
}