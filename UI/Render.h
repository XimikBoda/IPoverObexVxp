#pragma once
#include "vmgraph.h"

const unsigned short tr_color = VM_COLOR_888_TO_565(255, 0, 255);
extern int screen_w, screen_h;
extern "C" const unsigned char ProFont6x11[];
const int char_width = 6, char_height = 11;

namespace Render{
	void draw_text_white(unsigned short* buf, int x, int y, const char* str);
	void draw_text_white_centered(unsigned short* buf, int x, int y, int w, const char* str);
	void draw_text_white_by_len(unsigned short* buf, int x, int y, const char* str, int len);
	void draw_text_white_with_black_by_len(unsigned short* buf, int x, int y, const char* str, int len);

	void draw_fill_rect(unsigned short* buf, int x, int y, int w, int h, unsigned short color);
	void draw_fill_rect_with_borders(unsigned short* buf, int x, int y, int w, int h, unsigned short fill_color, unsigned short border_color);
};

