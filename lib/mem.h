#ifndef MEM_H
#define MEM_H

// Function prototype for peeking memory
static inline unsigned char peek_memory(unsigned long address);

// Function definition for peeking memory
static inline unsigned char peek_memory(unsigned long address) {
    unsigned char value;

    // Inline assembly to read a byte from the specified memory address
    __asm__ volatile (
        "mov %1, %%eax;"    // Move the address into EAX
        "movb (%%eax), %0;" // Move the byte at address (EAX) into value
        : "=r"(value)       // Output: value
        : "r"(address)      // Input: address
        : "%eax"            // Clobbered register
    );

    return value;
}

// Function prototype for poking memory
static inline void poke_memory(unsigned long address, unsigned char value);

// Function definition for poking memory
static inline void poke_memory(unsigned long address, unsigned char value) {
    // Inline assembly to write a byte to the specified memory address
    __asm__ volatile (
        "mov %1, %%eax;"    // Move the address into EAX
        "movb %0, (%%eax);" // Move the value into the address pointed by EAX
        :                     // No output
        : "r"(value), "r"(address) // Input: value and address
        : "%eax"            // Clobbered register
    );
}

#endif // MEM_H
