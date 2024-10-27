#include "pit.h"
#include <stdint.h>
#define PIT_COMMAND_PORT 0x43
#define PIT_CHANNEL0_PORT 0x40
#define SPEAKER_PORT 0x61

// Initialize the PIT
void pit_init() {
    // Set the PIT to operate in mode 2 (Rate Generator) and set the frequency
    unsigned short divisor = PIT_FREQUENCY / 100; // 10ms ticks for 100Hz
    outb(PIT_COMMAND_PORT, 0x36); // Command byte for PIT
    outb(PIT_CHANNEL0_PORT, divisor & 0xFF); // Low byte
    outb(PIT_CHANNEL0_PORT, (divisor >> 8) & 0xFF); // High byte
}

// Sleep function to delay execution for a specified number of milliseconds
void sleep(unsigned int ms) {
    unsigned int end_time = ms + ms; // Calculate end time (just a simple approach)
    while (end_time > 0) {
        // A simple busy-wait loop
        for (volatile int i = 0; i < 1000; i++); // Adjust for speed
        end_time--;
    }
}

static void play_sound(uint32_t nFrequence) {
    uint32_t Div;
    uint8_t tmp;

    // Set the PIT to the desired frequency
    Div = 1193180 / nFrequence;
    outb(0x43, 0xb6);
    outb(0x42, (uint8_t)(Div));      // Low byte
    outb(0x42, (uint8_t)(Div >> 8)); // High byte

    // And play the sound using the PC speaker
    tmp = inb(0x61);
    if ((tmp & 0x03) != 0x03) { // Check if the speaker is already enabled
        outb(0x61, tmp | 3); // Enable the speaker
    }
}

// Make it shut up
static void nosound() {
    uint8_t tmp = inb(0x61) & 0xFC; // Turn off the speaker
    outb(0x61, tmp);
}

// Make a beep
void beep(uint32_t frequency, uint32_t duration) {
    play_sound(frequency); // Play sound at the specified frequency
    sleep(duration); // Wait for the specified duration
    nosound(); // Stop the sound
}
