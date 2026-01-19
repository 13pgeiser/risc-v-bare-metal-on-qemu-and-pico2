#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h> // S_IFCHR

#define XOSC_BASE    0x40048000
#define XOSC_CTRL    (*(volatile uint32_t *)(XOSC_BASE + 0x00))
#define XOSC_STATUS  (*(volatile uint32_t *)(XOSC_BASE + 0x04))
#define XOSC_STARTUP (*(volatile uint32_t *)(XOSC_BASE + 0x0c))

#define RESETS_BASE       0x40020000
#define RESETS_RESET      (*(volatile uint32_t *)(RESETS_BASE + 0x00))
#define RESETS_RESET_DONE (*(volatile uint32_t *)(RESETS_BASE + 0x08))

#define PLL_SYS_BASE      0x40050000
#define PLL_SYS_CS        (*(volatile uint32_t *)(PLL_SYS_BASE + 0x00))
#define PLL_SYS_PWR       (*(volatile uint32_t *)(PLL_SYS_BASE + 0x04))
#define PLL_SYS_FBDIV_INT (*(volatile uint32_t *)(PLL_SYS_BASE + 0x08))
#define PLL_SYS_PRIM      (*(volatile uint32_t *)(PLL_SYS_BASE + 0x0c))

#define CLOCKS_BASE             0x40010000
#define CLOCKS_CLK_REF_CTRL     (*(volatile uint32_t *)(CLOCKS_BASE + 0x30))
#define CLOCKS_CLK_REF_SELECTED (*(volatile uint32_t *)(CLOCKS_BASE + 0x38))
#define CLOCKS_CLK_SYS_CTRL     (*(volatile uint32_t *)(CLOCKS_BASE + 0x3c))
#define CLOCKS_CLK_SYS_SELECTED (*(volatile uint32_t *)(CLOCKS_BASE + 0x44))
#define CLOCKS_CLK_PERI_CTRL    (*(volatile uint32_t *)(CLOCKS_BASE + 0x48))

void setup_clocks(void) {
    XOSC_STARTUP = (XOSC_STARTUP & ~0x00003fff) | 469;
    XOSC_CTRL = (XOSC_CTRL & ~0x00ffffff) | 0xfabaa0;
    while (!(XOSC_STATUS & (1U << 31)));
    RESETS_RESET &= ~(1u << 14);
    while (!(RESETS_RESET_DONE & (1 << 14)));
    PLL_SYS_FBDIV_INT = (PLL_SYS_FBDIV_INT & ~0x00000fff) | 125;
    PLL_SYS_PWR &= ~((1 << 5) | (1 << 0));
    while (!(PLL_SYS_CS & (1U << 31)));
    PLL_SYS_PRIM = (PLL_SYS_PRIM & ~0x00077000) | (5 << 16) | (2 << 12);
    PLL_SYS_PWR &= ~(1 << 3);
    CLOCKS_CLK_REF_CTRL = (CLOCKS_CLK_REF_CTRL & ~0x00000003) | 0x2;
    while (!(CLOCKS_CLK_REF_SELECTED & 0x00f) == 0b0100);
    CLOCKS_CLK_SYS_CTRL = (0x0 << 5) | (0x1 << 0);
    while (!(CLOCKS_CLK_SYS_SELECTED & (1 << 1)));
}

// RP2350 Specific Base Addresses
#define UART0_BASE       0x40070000
#define RESETS_BASE      0x40020000
#define IO_BANK0_BASE    0x40028000
#define PADS_BANK0_BASE  0x40038000

// UART Registers (PL011)
#define UART_DR          (*(volatile uint32_t *)(UART0_BASE + 0x00))
#define UART_FR          (*(volatile uint32_t *)(UART0_BASE + 0x18))
#define UART_IBRD        (*(volatile uint32_t *)(UART0_BASE + 0x24))
#define UART_FBRD        (*(volatile uint32_t *)(UART0_BASE + 0x28))
#define UART_LCR_H       (*(volatile uint32_t *)(UART0_BASE + 0x2c))
#define UART_CR          (*(volatile uint32_t *)(UART0_BASE + 0x30))

// Reset Control Registers
#define RESET_CTRL       (*(volatile uint32_t *)(RESETS_BASE + 0x00))
#define RESET_DONE       (*(volatile uint32_t *)(RESETS_BASE + 0x08))

// GPIO & Pad Control
#define GPIO0_CTRL       (*(volatile uint32_t *)(IO_BANK0_BASE + 0x04))
#define GPIO1_CTRL       (*(volatile uint32_t *)(IO_BANK0_BASE + 0x0c))
#define PAD_GPIO0        (*(volatile uint32_t *)(PADS_BANK0_BASE + 0x04))
#define PAD_GPIO1        (*(volatile uint32_t *)(PADS_BANK0_BASE + 0x08))

void uart_init_rp2350() {
  CLOCKS_CLK_PERI_CTRL = (1 << 11); // ENABLE bit
  // Release Resets: UART0 (bit 26), IO_BANK0 (bit 6), PADS_BANK0 (bit 9)
  uint32_t mask = (1 << 26) | (1 << 6) | (1 << 9);
  RESET_CTRL &= ~mask;
  while ((RESET_DONE & mask) != mask);
  // Disable UART before configuration
  UART_CR = 0;
  // Configure PADS (Physical Electrical Properties)
  PAD_GPIO0 = 0x30;
  // Set GPIO Function 2 (UART0)
  GPIO0_CTRL = 2;
  // Baud Rate: 115200 at 150MHz clk_peri
  // Divisor = 150,000,000 / (16 * 115200) = 81.3802
  UART_IBRD = 81;
  UART_FBRD = 24;
  // Setup Line Control & Enable (8N1, FIFOs Enabled)
  UART_LCR_H = (0x3 << 5) | (1 << 4);
  UART_CR = (1 << 8) | (1 << 0); // TXE, UARTEN
}

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
    int written = 0;
    for (; len != 0; --len) {
         // Wait until TX FIFO is not full (FR bit 5 = TXFF)
        while (UART_FR & (1 << 5));
        UART_DR = *ptr++;
        written ++;
    }
    return written;
}

int main(int argc, char* argv[]) {
  setup_clocks();
  uart_init_rp2350();
  const char* message = "Hello from RISC-V virtual implementation running on Pico2!\n";
  puts(message);
  return 0;
}
