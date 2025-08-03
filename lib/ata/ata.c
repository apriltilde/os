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
        return;
    }

    timeout = 100000;
    while (!(inb(ATA_STATUS) & ATA_SR_DRQ) && --timeout);
    if (timeout == 0) {
    }
}

static void ata_flush() {
    outb(ATA_COMMAND, ATA_FLUSH_CACHE);
    ata_wait();
}

static void io_wait() {
    outb(0x80, 0); // small delay
}

uint32_t hex_to_uint32(const char *hex) {
    uint32_t value = 0;
    while (*hex) {
        char c = *hex++;
        value <<= 4;
        if (c >= '0' && c <= '9') value += c - '0';
        else if (c >= 'A' && c <= 'F') value += c - 'A' + 10;
        else if (c >= 'a' && c <= 'f') value += c - 'a' + 10;
        else break;
    }
    return value;
}

void uint32_to_hex(uint32_t value, char *out) {
    const char *hex_digits = "0123456789ABCDEF";

    if (value == 0) {
        out[0] = '0';
        out[1] = '\0';
        return;
    }

    int pos = 0;
    int started = 0;

    for (int i = 28; i >= 0; i -= 4) {
        uint8_t digit = (value >> i) & 0xF;
        if (digit != 0 || started) {
            out[pos++] = hex_digits[digit];
            started = 1;
        }
    }

    out[pos] = '\0';
}

void read_sector(uint8_t* buffer, uint32_t lba) {
    newline();

    outb(ATA_SECCOUNT0, 1);
    outb(ATA_LBA0, lba & 0xFF);
    outb(ATA_LBA1, (lba >> 8) & 0xFF);
    outb(ATA_LBA2, (lba >> 16) & 0xFF);
    outb(ATA_DRIVE, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_COMMAND, ATA_READ_CMD);

    ata_wait();

    // Read 256 words (512 bytes)
    for (int i = 0; i < 256; i++) {
        uint16_t data = inw(ATA_DATA);
        buffer[i * 2] = data & 0xFF;
        buffer[i * 2 + 1] = (data >> 8) & 0xFF;

    }

}

void write_sector(const uint8_t* buffer, uint32_t lba) {
    newline();

    // Set up LBA
    outb(ATA_SECCOUNT0, 1);
    outb(ATA_LBA0, lba & 0xFF);
    outb(ATA_LBA1, (lba >> 8) & 0xFF);
    outb(ATA_LBA2, (lba >> 16) & 0xFF);
    outb(ATA_DRIVE, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_COMMAND, ATA_WRITE_CMD);

    ata_wait();

    // Write 256 words (512 bytes)
    for (int i = 0; i < 256; i++) {
        uint16_t word = buffer[i * 2] | (buffer[i * 2 + 1] << 8);
        outw(ATA_DATA, word);
    }

    ata_flush();
}




