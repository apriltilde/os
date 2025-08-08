#include "../core/print.h"
#include "../core/io.h"
#include "clock.h"
#include "../vga/vga.h"

extern struct bitmap_font font;
#define RTC_INDEX_PORT 0x70
#define RTC_DATA_PORT  0x71

#define RTC_REG_SECONDS    0x00
#define RTC_REG_MINUTES    0x02
#define RTC_REG_HOURS      0x04
#define RTC_REG_DAY        0x07
#define RTC_REG_MONTH      0x08
#define RTC_REG_YEAR       0x09
#define RTC_REG_STATUS_B   0x0B

static unsigned char rtc_read(unsigned char reg) {
    outb(RTC_INDEX_PORT, reg);
    return inb(RTC_DATA_PORT);
}

static unsigned char bcd_to_bin(unsigned char val) {
    return (val & 0x0F) + ((val >> 4) * 10);
}

void read_rtc_time(int *hours, int *minutes, int *seconds) {
    while (rtc_read(0x0A) & 0x80) {}

    unsigned char sec = rtc_read(RTC_REG_SECONDS);
    unsigned char min = rtc_read(RTC_REG_MINUTES);
    unsigned char hr  = rtc_read(RTC_REG_HOURS);
    unsigned char status_b = rtc_read(RTC_REG_STATUS_B);

    if (!(status_b & 0x04)) {
        sec = bcd_to_bin(sec);
        min = bcd_to_bin(min);
        hr  = bcd_to_bin(hr);
    }

    if (!(status_b & 0x02) && (hr & 0x80)) {
        hr = ((hr & 0x7F) + 12) % 24;
    }

    *hours = hr;
    *minutes = min;
    *seconds = sec;
}

void print_time(void) {
    int h, m, s;
    read_rtc_time(&h, &m, &s);

    char buf[9];
    buf[0] = '0' + (h / 10);
    buf[1] = '0' + (h % 10);
    buf[2] = ':';
    buf[3] = '0' + (m / 10);
    buf[4] = '0' + (m % 10);
    buf[5] = ':';
    buf[6] = '0' + (s / 10);
    buf[7] = '0' + (s % 10);
    buf[8] = '\0';

    print(WHITE, "\n");
    print(WHITE, buf);
    print(WHITE, "\n");
}

void read_rtc_date(int *year, int *month, int *day) {
    while (rtc_read(0x0A) & 0x80) {}

    unsigned char d = rtc_read(RTC_REG_DAY);
    unsigned char m = rtc_read(RTC_REG_MONTH);
    unsigned char y = rtc_read(RTC_REG_YEAR);
    unsigned char status_b = rtc_read(RTC_REG_STATUS_B);

    if (!(status_b & 0x04)) {
        d = bcd_to_bin(d);
        m = bcd_to_bin(m);
        y = bcd_to_bin(y);
    }

    *day = d;
    *month = m;
    *year = y + 2000; // RTC year is typically offset from 2000
}

void print_date(void) {
    int y, m, d;
    read_rtc_date(&y, &m, &d);

	for (int i = 0; i < 10; i++) {
		putpixel(i, 10, black);
	}

    char buf[11]; // "YYYY-MM-DD\0"
    buf[0] = '0' + (y / 1000);
    buf[1] = '0' + ((y / 100) % 10);
    buf[2] = '0' + ((y / 10) % 10);
    buf[3] = '0' + (y % 10);
    buf[4] = '-';
    buf[5] = '0' + (m / 10);
    buf[6] = '0' + (m % 10);
    buf[7] = '-';
    buf[8] = '0' + (d / 10);
    buf[9] = '0' + (d % 10);
    buf[10] = '\0';

    print(WHITE, "\n");
    print(WHITE, buf);
    print(WHITE, "\n");
}


void vga_print_date(void) {
    static int last_y = -1, last_m = -1, last_d = -1; // store last printed date

    int y, m, d;
    read_rtc_date(&y, &m, &d);

    // Only update if the date changed
    if (y == last_y && m == last_m && d == last_d) {
        return;
    }

    last_y = y;
    last_m = m;
    last_d = d;

    // Clear old date area
    for (int i = 0; i < 60; i++) {
        for (int j = 10; j < 17; j++) {
            putpixel(i, j, black);  
        }
    }

    char buf[11]; // "YYYY-MM-DD\0"
    buf[0] = '0' + (y / 1000);
    buf[1] = '0' + ((y / 100) % 10);
    buf[2] = '0' + ((y / 10) % 10);
    buf[3] = '0' + (y % 10);
    buf[4] = '-';
    buf[5] = '0' + (m / 10);
    buf[6] = '0' + (m % 10);
    buf[7] = '-';
    buf[8] = '0' + (d / 10);
    buf[9] = '0' + (d % 10);
    buf[10] = '\0';

    putstring(0, 20, buf, &font, white);
}

void vga_print_time(void) {
    static int last_h = -1, last_m = -1, last_s = -1;
    static int call_count = 0; // counts successful time updates

    int h, m, s;
    read_rtc_time(&h, &m, &s);

    // If time hasn't changed, skip update
    if (h == last_h && m == last_m && s == last_s) {
        return;
    }

    // Save new time
    last_h = h;
    last_m = m;
    last_s = s;

    // Clear old time area
    for (int i = 0; i < 40; i++) {
        for (int j = 18; j < 25; j++) {
            putpixel(i, j, black);  
        }
    }

    char buf[9];
    buf[0] = '0' + (h / 10);
    buf[1] = '0' + (h % 10);
    buf[2] = ':';
    buf[3] = '0' + (m / 10);
    buf[4] = '0' + (m % 10);
    buf[5] = ':';
    buf[6] = '0' + (s / 10);
    buf[7] = '0' + (s % 10);
    buf[8] = '\0';

    putstring(0, 28, buf, &font, white);

    // Increment counter for date updates
    call_count++;
    if (call_count >= 10) {
        vga_print_date(); // this already only updates once per day
        call_count = 0;
    }
}

