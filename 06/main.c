#define NS16550_BASE_ADDR (0x10000000)
#define NS16550_THR (NS16550_BASE_ADDR + 0x00)
#define NS16550_IER (NS16550_BASE_ADDR + 0x01)
#define NS16550_IER_THR_EMPTY (1 << 1)

void uart_write(const char* ptr) {
    unsigned char* ns16550_ier = (unsigned char*) NS16550_IER;
    char* ns16550_thr = (char*)NS16550_THR;

    while (*ptr != '\0') {
        while (*ns16550_ier & NS16550_IER_THR_EMPTY);
        *ns16550_thr = *ptr++;
    }
}

int main(int argc, char* argv[]) {
    const char* message = "Hello from RISC-V virtual implementation running in QEMU!\n";
    uart_write(message);
    return 0;
}
