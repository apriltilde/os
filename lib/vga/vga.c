#include <stdint.h>
#include "../core/io.h"
#include "font/font.h"
#include "vga.h"
#include "../clock/clock.h"

extern const struct bitmap_font font;
// Bochs VBE registers and constants
#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

#define PCI_VENDOR_ID 0x0
#define PCI_DEVICE_ID 0x2

#define BGA_VENDOR_ID 0x1234
#define BGA_DEVICE_ID 0x1111

#define VBE_DISPI_IOPORT_INDEX  0x01CE
#define VBE_DISPI_IOPORT_DATA   0x01CF

// VBE Registers
#define VBE_DISPI_INDEX_ID           0x0
#define VBE_DISPI_INDEX_XRES         0x1
#define VBE_DISPI_INDEX_YRES         0x2
#define VBE_DISPI_INDEX_BPP          0x3
#define VBE_DISPI_INDEX_ENABLE       0x4
#define VBE_DISPI_INDEX_BANK         0x5
#define VBE_DISPI_INDEX_VIRT_WIDTH   0x6
#define VBE_DISPI_INDEX_VIRT_HEIGHT  0x7
#define VBE_DISPI_INDEX_X_OFFSET     0x8
#define VBE_DISPI_INDEX_Y_OFFSET     0x9

// VBE Enable flags
#define VBE_DISPI_DISABLED           0x00
#define VBE_DISPI_ENABLED            0x01
#define VBE_DISPI_LFB_ENABLED        0x40
#define VBE_DISPI_NOCLEARMEM         0x80

// VGA PCI info
#define PCI_BUS  0
#define PCI_SLOT 0
#define PCI_FUNC 0

// Colors
static inline void BgaWriteRegister(uint16_t index, uint16_t value) {
    outw(VBE_DISPI_IOPORT_INDEX, index);
    outw(VBE_DISPI_IOPORT_DATA, value);
}

static inline uint16_t BgaReadRegister(uint16_t index) {
    outw(VBE_DISPI_IOPORT_INDEX, index);
    return inw(VBE_DISPI_IOPORT_DATA);
}

static uint32_t pciConfigRead(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address = (1U << 31) | (bus << 16) | (slot << 11) | (func << 8) | (offset & 0xFC);
    outl(PCI_CONFIG_ADDRESS, address);
    return inl(PCI_CONFIG_DATA);
}

static void pciConfigWrite(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t value) {
    uint32_t address = (1U << 31) | (bus << 16) | (slot << 11) | (func << 8) | (offset & 0xFC);
    outl(PCI_CONFIG_ADDRESS, address);
    outl(PCI_CONFIG_DATA, value);
}

// Find PCI device with given vendor/device ID
static int findBgaDevice(uint8_t *outBus, uint8_t *outSlot, uint8_t *outFunc) {
    for (uint8_t bus = 0; bus < 256; bus++) {
        for (uint8_t slot = 0; slot < 32; slot++) {
            for (uint8_t func = 0; func < 8; func++) {
                uint32_t vendorDevice = pciConfigRead(bus, slot, func, PCI_VENDOR_ID);
                uint16_t vendor = vendorDevice & 0xFFFF;
                uint16_t device = (vendorDevice >> 16) & 0xFFFF;
                if (vendor == BGA_VENDOR_ID && device == BGA_DEVICE_ID) {
                    *outBus = bus;
                    *outSlot = slot;
                    *outFunc = func;
                    return 1; // found
                }
                if (func == 0) break; // single function device optimization
            }
        }
    }
    return 0; // not found
}

static volatile uint32_t *framebuffer = 0;
static uint16_t screenWidth = 0;
static uint16_t screenHeight = 0;



// Draw a pixel at (x,y) with given ARGB color
void putpixel(uint16_t x, uint16_t y, uint32_t color) {
    if (!framebuffer) return;
    if (x >= screenWidth || y >= screenHeight) return;

    framebuffer[y * screenWidth + x] = color;
}

// Optional: Draw a simple test pattern
void vga_test_pattern() {
    if (!framebuffer) return;
    for (uint16_t y = 0; y < screenHeight; y++) {
        for (uint16_t x = 0; x < screenWidth; x++) {
            uint32_t r = (x * 255) / screenWidth;
            uint32_t g = (y * 255) / screenHeight;
            uint32_t b = 128;
            uint32_t color = (0xFF << 24) | (r << 16) | (g << 8) | b;
            putpixel(x, y, color);
        }
    }
}
void putchar(int x, int y, char c, const struct bitmap_font* font, uint32_t color) {
    if (!font || !font->Bitmap || !font->Index) return;

    unsigned char uc = (unsigned char)c;

    // Find glyph index
    int glyph_index = -1;
    for (int i = 0; i < font->Chars; ++i) {
        if (font->Index[i] == uc) {
            glyph_index = i;
            break;
        }
    }
    if (glyph_index == -1) return;

    int width = font->Widths ? font->Widths[glyph_index] : font->Width;
    int height = font->Height;

    const unsigned char* bitmap = &font->Bitmap[glyph_index * height];

    // Draw from the bottom up, vertically aligned to baseline
    for (int row = 0; row < height; ++row) {
        unsigned char row_bits = bitmap[row];
        for (int col = 0; col < width && col < 8; ++col) {
            if (row_bits & (1 << (7 - col))) {
                putpixel(x + col, y + row, color);
            }
        }
    }
}


void putstring(int x, int y, const char* str, const struct bitmap_font* font, uint32_t color) {
    if (!str || !font) return;

    int cursor_x = x;

    while (*str) {
        char c = *str++;

        int glyph_index = -1;
        for (int i = 0; i < font->Chars; ++i) {
            if (font->Index[i] == (unsigned char)c) {
                glyph_index = i;
                break;
            }
        }

        if (glyph_index == -1) continue;

        int char_width = font->Widths ? font->Widths[glyph_index] : font->Width;

        // Draw character, aligned to baseline
		putchar(cursor_x, y - font->Height, c, font, color);

        cursor_x += char_width + 1;
    }
}

static void int_to_str(int value, char* buffer, int buffer_size) {
    if (buffer_size <= 0) return;

    int i = buffer_size - 1;
    buffer[i] = '\0';
    i--;

    int is_negative = 0;
    unsigned int uvalue;

    if (value < 0) {
        is_negative = 1;
        uvalue = (unsigned int)(-value);
    } else {
        uvalue = (unsigned int)value;
    }

    if (uvalue == 0) {
        if (i >= 0) {
            buffer[i] = '0';
            i--;
        }
    } else {
        while (uvalue > 0 && i >= 0) {
            buffer[i] = '0' + (uvalue % 10);
            uvalue /= 10;
            i--;
        }
    }

    if (is_negative && i >= 0) {
        buffer[i] = '-';
        i--;
    }

    // Shift string to start of buffer
    int start = i + 1;
    int j = 0;
    while (buffer[start] != '\0' && j < buffer_size) {
        buffer[j] = buffer[start];
        j++; start++;
    }
    buffer[j] = '\0';
}

// Print an integer at (x,y) in given color and font
void putint(int x, int y, int value, const struct bitmap_font* font, uint32_t color) {
    char buffer[12]; // Enough for 32-bit int plus sign and null terminator
    int_to_str(value, buffer, sizeof(buffer));
    putstring(x, y, buffer, font, color);
}


// Initialize Bochs VBE graphics mode 1024x768 32bpp
void initvideo() {
    uint8_t bus, slot, func;
    if (!findBgaDevice(&bus, &slot, &func)) {
        // VGA device not found
        return;
    }

    // Read PCI BAR0 for framebuffer address
    uint32_t bar0 = pciConfigRead(bus, slot, func, 0x10);
    // Mask to get physical address (bits 31..4)
    uint32_t fb_address = bar0 & 0xFFFFFFF0;

    // Disable VBE before mode setting
    BgaWriteRegister(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);

    // Set resolution and bpp
    screenWidth = 1024;
    screenHeight = 768;

    BgaWriteRegister(VBE_DISPI_INDEX_XRES, screenWidth);
    BgaWriteRegister(VBE_DISPI_INDEX_YRES, screenHeight);
    BgaWriteRegister(VBE_DISPI_INDEX_BPP, 32);

    // Enable VBE with linear framebuffer
    BgaWriteRegister(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);

    // Assign framebuffer pointer (identity mapped)
    framebuffer = (volatile uint32_t *)fb_address;

    // Clear screen to black
	for (uint32_t i = 0; i < screenWidth * screenHeight; i++) {
        framebuffer[i] = blue;
    }
	
	for (uint32_t i = 0; i < screenWidth * screenHeight; i++) {
		framebuffer[i] = red;
	}
	vga_test_pattern();
	putstring(0, 12, "APRILoS", &font, white);
}


void exitvideo() {
    if (!framebuffer) return;

    // Disable VBE graphics mode
    BgaWriteRegister(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);

    framebuffer = 0;
    screenWidth = 0;
    screenHeight = 0;
}

#define TEXT_MODE_FB ((volatile uint16_t*)0xB8000)
#define TEXT_COLS 80
#define TEXT_ROWS 25

void clear_text_mode_screen(uint8_t attr) {
    for (int i = 0; i < TEXT_COLS * TEXT_ROWS; i++) {
        TEXT_MODE_FB[i] = (attr << 8) | ' ';
    }
}



#define KEYBOARD_DATA_PORT 0x60
#define KEY_A_PRESSED 0x1E

void keyboard_poll() {
    static int cursor_x = 100;
    static int cursor_y = 100;

    if (inb(0x64) & 1) { // Check if data is available
        uint8_t scancode = inb(KEYBOARD_DATA_PORT);

        if (scancode == KEY_A_PRESSED) {
            putchar(cursor_x, cursor_y, 'a', &font, white);
            cursor_x += font.Width + 1;
        }
    }
}

