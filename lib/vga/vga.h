#ifndef VGA_H
#define VGA_H

#define black 0x00000000
#define white 0xFFFFFFFF
#define red   0xFF000000
#define green 0x00FF0000
#define blue  0x0000FF00
#define yellow 0xFFFF0000
#define magenta 0xFF00FF00
#define cyan 0x0000FFFF
#define light_gray 0xFFC0C0C0
#define light_red 0xFFFFC0C0
#define light_green 0xC0FFC0C0
#define light_cyan 0xC0C0FFFF



struct bitmap_font;

void initvideo(void);
void putpixel(uint16_t x, uint16_t y, uint32_t color);
void putchar(int x, int y, char c, const struct bitmap_font* font, uint32_t color);
void putstring(int x, int y, const char* str, const struct bitmap_font* font, uint32_t color);
void putint(int x, int y, int value, const struct bitmap_font* font, uint32_t color);
void delchar(int x, int y, const struct bitmap_font* font);
void vga_test_pattern(void);
void redraw_screen(void);

#endif
