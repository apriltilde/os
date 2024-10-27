#ifndef PIT_TIMER_H
#define PIT_TIMER_H

#include "io.h"

// PIT constants
#define PIT_COMMAND_PORT 0x43
#define PIT_CHANNEL0_PORT 0x40
#define PIT_FREQUENCY 1193182  // PIT frequency in Hz

// Function to initialize the PIT
void pit_init();
void sleep(unsigned int ms);
void beep(unsigned int freq, unsigned int time);

#endif
