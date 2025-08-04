#ifndef VGA_H
#define VGA_H

#define black 0x00000000
#define white 0xFFFFFFFF
#define red   0xFF000000
#define green 0x00FF0000
#define blue  0x0000FF00

void initvideo(void);
void putpixel(int x, int y, uint32_t color);
void putstring(int x, int y, char* str);
void test_graphics(void);

#endif
