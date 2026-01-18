#include <errno.h>
#include <stdio.h>
#include <sys/stat.h> // S_IFCHR

#define NS16550_BASE_ADDR (0x10000000)
#define NS16550_THR (NS16550_BASE_ADDR + 0x00)
#define NS16550_IER (NS16550_BASE_ADDR + 0x01)
#define NS16550_IER_THR_EMPTY (1 << 1)

int _close(int file) {
  return -1;
}

int _fstat(int file, struct stat *st) {
  st->st_mode = S_IFCHR;
  return 0;
}

int _isatty(int file) {
  return 1;
}

int _lseek(int file, int ptr, int dir) {
  return 0;
}

int _read (int file, char * ptr, int len) {
  return -1;
}

void *_sbrk (int  incr) {
	extern char _memory_start;
	extern char _memory_end;
	static char *heap_end = 0;
	char        *prev_heap_end;
	if (0 == heap_end) {
		heap_end = &_memory_start;
	}
	prev_heap_end  = heap_end;
	heap_end      += incr;
	if( heap_end >= (&_memory_end)) {
		errno = ENOMEM;
		return (char*)-1;
	}
	return (void *) prev_heap_end;
}	

int _write(int file, char * ptr, int len) {
    if ((file != 1) && (file != 2) && (file != 3)) {
        return -1;
    }
    unsigned char* ns16550_ier = (unsigned char*) NS16550_IER;
    char* ns16550_thr = (char*)NS16550_THR;
    int written = 0;
    for (; len != 0; --len) {
        while (*ns16550_ier & NS16550_IER_THR_EMPTY);
        *ns16550_thr = *ptr++;
        written ++;
    }
    return written;
}

int main(int argc, char* argv[]) {
    const char* message = "Hello from RISC-V virtual implementation running in QEMU!\n";
    puts(message);
    return 0;
}
