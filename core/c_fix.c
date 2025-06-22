#ifndef WIN32
#include <vmstdlib.h>
#include <vmio.h>
#include <stdio.h>
#include <errno.h>

#include <console.h>

void _exit(int c) {
	cprintf("ERROR: _exit(%d)\n", c);
	vm_exit_app();
	void (*f)() = 0;
	f();
}
void abort() {
	cprintf("ERROR: _abort()\n");
	_exit(0);
}

void __assert_func(const char* file, int line, const char* func, const char* msg) {
	cprintf("ERROR: __assert_func(%s, %d, %%, %s)\n", file, line, func, msg);
	_exit(0);
}

int __wrap_sprintf(char* buffer, const char* format, ...) {
	va_list aptr;
	int ret;

	va_start(aptr, format);
	ret = vsprintf(buffer, format, aptr);
	va_end(aptr);

	return(ret);
}


int _read(int file, char* ptr, int len) {
	return 0;
}

int _write(int file, char* ptr, int len) {
	return len;
}

int _open(const char* name, int flags, int mode) {
	errno = ENOSYS;
	return -1;
}

int _close(int file) {
	errno = ENOSYS;
	return -1;
}

int _lseek(int file, int ptr, int dir) {
	errno = ENOSYS;
	return -1;
}

int _fstat(int file, struct stat* st) {
	return 0;
}

int _isatty(int file) {
	return 1;
}

int _kill(int pid, int sig) {
	errno = EINVAL;
	return -1;
}

int _getpid(void) {
	return 1;
}
#endif // !WIN32