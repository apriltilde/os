#ifndef CLOCK_H
#define CLOCK_H

void read_rtc_time(int *hours, int *minutes, int *seconds);
void print_time(void);

void read_rtc_date(int *year, int *month, int *day);
void print_date(void);

void vga_print_date(int);
void vga_print_time(void);

#endif // CLOCK_H

