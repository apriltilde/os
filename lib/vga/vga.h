#ifndef VGA_H
#define VGA_H

#define white 0xFFFFFFFF
#define green 0xFF00FF00 
#define red   0xFFFF0000 
#define blue  0xFF0000FF 
#define black 0x00000000



struct bitmap_font;

void initvideo(void);
void putpixel(uint16_t x, uint16_t y, uint32_t color);
void putchar(int x, int y, char c, const struct bitmap_font* font, uint32_t color);
void putstring(int x, int y, const char* str, const struct bitmap_font* font, uint32_t color);
void putint(int x, int y, int value, const struct bitmap_font* font, uint32_t color);
void delchar(int x, int y, const struct bitmap_font* font);
void vga_test_pattern(void);
void redraw_screen(void);
void exitgraphics(void);
#endif
