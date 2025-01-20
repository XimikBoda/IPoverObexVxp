#pragma once
#ifdef __cplusplus
extern "C" {
#endif

void console_init(int _scr_w, int _scr_h, unsigned short* _scr_buf);
void console_put_char(char c);
void console_put_str(const char* str);
int cprintf(char const* const format, ...);

#ifdef __cplusplus
}
#endif