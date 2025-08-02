#ifndef MEM_H
#define MEM_H

#include "../core/print.h"

// Function prototype for peeking memory
static inline unsigned char peek_memory(unsigned long address);

// Function definition for peeking memory
static inline unsigned char peek_memory(unsigned long address) {
    unsigned char value;

    __asm__ volatile (
        "mov %1, %%eax;"
        "movb (%%eax), %0;"
        : "=r"(value)
        : "r"(address)
        : "%eax"
    );

    return value;
}

// Function prototype for poking memory
static inline void poke_memory(unsigned long address, unsigned char value);

// Function definition for poking memory
static inline void poke_memory(unsigned long address, unsigned char value) {
    __asm__ volatile (
        "mov %1, %%eax;"
        "movb %0, (%%eax);"
        :
        : "r"(value), "r"(address)
        : "%eax"
    );
}

// Convert hex string (e.g., "0x1F4") to unsigned long
static inline unsigned long parse_hex_address(const char* str) {
    unsigned long result = 0;
    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        str += 2; // Skip '0x'
    }
    while (*str) {
        char c = *str++;
        result <<= 4;
        if (c >= '0' && c <= '9') result += c - '0';
        else if (c >= 'a' && c <= 'f') result += c - 'a' + 10;
        else if (c >= 'A' && c <= 'F') result += c - 'A' + 10;
        else break; // Stop on invalid character
    }
    return result;
}

#endif // MEM_H

