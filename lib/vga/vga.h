#ifndef VGA_H
#define VGA_H

#define black 0x00000000
#define white 0xFFFFFFFF
#define red   0xFF000000
#define green 0x00FF0000
#define blue  0x0000FF00


struct bitmap_font;

void initvideo(void);
void putpixel(uint16_t x, uint16_t y, uint32_t color);
void putchar(int x, int y, char c, const struct bitmap_font* font, uint32_t color);
void putstring(int x, int y, const char* str, const struct bitmap_font* font, uint32_t color);
void putint(int x, int y, int value, const struct bitmap_font* font, uint32_t color);
void test_graphics(void);
void keyboard_poll(void);

#endif
