#include "../core/io.h"
#include "../core/print.h"
#include <stdint.h>

#define ATA_DATA        0x1F0
#define ATA_ERROR       0x1F1
#define ATA_SECCOUNT0   0x1F2
#define ATA_LBA0        0x1F3
#define ATA_LBA1        0x1F4
#define ATA_LBA2        0x1F5
#define ATA_DRIVE       0x1F6
#define ATA_COMMAND     0x1F7
#define ATA_STATUS      0x1F7

#define ATA_READ_CMD     0x20
#define ATA_WRITE_CMD    0x30
#define ATA_FLUSH_CACHE  0xE7

#define ATA_SR_BSY      0x80
#define ATA_SR_DRQ      0x08

static void ata_wait() {
    int timeout = 100000;
    while ((inb(ATA_STATUS) & ATA_SR_BSY) && --timeout);
    if (timeout == 0) {
        print(RED, "Timeout waiting for BSY to clear!\n");
        return;
    }

    timeout = 100000;
    while (!(inb(ATA_STATUS) & ATA_SR_DRQ) && --timeout);
    if (timeout == 0) {
        print(RED, "Timeout waiting for DRQ to set!\n");
    }
}

static void ata_flush() {
    outb(ATA_COMMAND, ATA_FLUSH_CACHE);
    ata_wait();
}

static void io_wait() {
    outb(0x80, 0); // small delay
}

void read(void) {
    uint8_t* buffer = (uint8_t*)0x7E00;

    print(LIGHT_CYAN, "Reading sector 5 first 8 bytes...\n");

    outb(ATA_SECCOUNT0, 1);
    outb(ATA_LBA0, 5 & 0xFF);
    outb(ATA_LBA1, (5 >> 8) & 0xFF);
    outb(ATA_LBA2, (5 >> 16) & 0xFF);
    outb(ATA_DRIVE, 0xE0 | ((5 >> 24) & 0x0F));
    outb(ATA_COMMAND, ATA_READ_CMD);

    ata_wait();

    // Read the entire 512-byte sector (256 words)
    for (int i = 0; i < 256; i++) {
        uint16_t data = inw(ATA_DATA);
        buffer[i * 2] = data & 0xFF;
        buffer[i * 2 + 1] = (data >> 8) & 0xFF;

        if (i < 4) {  // Only print first 8 bytes
            print(LIGHT_GREEN, "Read word: 0x");
            print_hex(WHITE, data);
            newline();
        }
    }

    print(LIGHT_CYAN, "Read complete.\n");
}

void ata_write_sector5_first8() {
    uint8_t dummy_data[8] = { 'U', 'U', 'M', 'M', 'Y', 'D', 'A', 'T' };
    uint8_t read_back[8];

    print(LIGHT_MAGENTA, "Writing sector 5 first 8 bytes...\n");

    // Issue write
    outb(ATA_SECCOUNT0, 1);
    outb(ATA_LBA0, 5 & 0xFF);
    outb(ATA_LBA1, (5 >> 8) & 0xFF);
    outb(ATA_LBA2, (5 >> 16) & 0xFF);
    outb(ATA_DRIVE, 0xE0 | ((5 >> 24) & 0x0F));
    outb(ATA_COMMAND, ATA_WRITE_CMD);

    ata_wait();

    // Write 8 bytes of actual data
    for (int i = 0; i < 4; i++) {
        uint16_t word = dummy_data[i * 2] | (dummy_data[i * 2 + 1] << 8);
        outw(ATA_DATA, word);

        print(GREEN, "Wrote word: 0x");
        print_hex(WHITE, word);
        newline();
    }

    // Pad the rest of the sector (252 words = 504 bytes) with zeros
    for (int i = 0; i < 252; i++) {
        outw(ATA_DATA, 0x0000);
    }

    ata_flush();
    print(LIGHT_MAGENTA, "Write and flush complete.\n");

    // Read back for verification
    print(LIGHT_CYAN, "Reading sector 5 for verification...\n");

    outb(ATA_SECCOUNT0, 1);
    outb(ATA_LBA0, 5 & 0xFF);
    outb(ATA_LBA1, (5 >> 8) & 0xFF);
    outb(ATA_LBA2, (5 >> 16) & 0xFF);
    outb(ATA_DRIVE, 0xE0 | ((5 >> 24) & 0x0F));
    outb(ATA_COMMAND, ATA_READ_CMD);

    ata_wait();

    // Read full sector
    for (int i = 0; i < 256; i++) {
        uint16_t data = inw(ATA_DATA);
        if (i < 4) {
            read_back[i * 2] = data & 0xFF;
            read_back[i * 2 + 1] = (data >> 8) & 0xFF;

            print(LIGHT_GREEN, "Read word: 0x");
            print_hex(WHITE, data);
            newline();
        }
    }

    // Verify
    print(LIGHT_CYAN, "Verifying written data...\n");
    for (int i = 0; i < 8; i++) {
        if (read_back[i] != dummy_data[i]) {
            print(RED, "Mismatch at byte ");
            print_char(WHITE, '0' + i);
            print(RED, ": expected ");
            print_char(WHITE, dummy_data[i]);
            print(RED, ", got ");
            print_char(WHITE, read_back[i]);
            newline();
        } else {
            print(LIGHT_GREEN, "Byte ");
            print_char(WHITE, '0' + i);
            print(LIGHT_GREEN, " verified: ");
            print_char(WHITE, read_back[i]);
            newline();
        }
    }

    print(LIGHT_CYAN, "Verification complete.\n");
}

