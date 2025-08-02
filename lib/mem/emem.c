// mem.c

#include "../core/print.h"
#include "mem.h"
#include "../keyboard/keyboard.h"

#define BUFFER_SIZE 128

extern char input_buffer[BUFFER_SIZE];
extern int extract_arguments(const char *command, char args[][BUFFER_SIZE], int max_args, int max_len);
extern unsigned long parse_hex_address(const char *str);
extern unsigned char peek_memory(unsigned long address);
extern void poke_memory(unsigned long address, unsigned char value);

void peek_command(void) {
    char args[1][BUFFER_SIZE];
    int count = extract_arguments("peek", args, 1, BUFFER_SIZE);

    if (count < 1 || args[0][0] == '\0') {
        print(WHITE, "\nUsage: peek 0xADDRESS\n");
        return;
    }

    unsigned long address = parse_hex_address(args[0]);
    unsigned char value = peek_memory(address);

    char output[3];
    const char *hex = "0123456789ABCDEF";
    output[0] = hex[(value >> 4) & 0xF];
    output[1] = hex[value & 0xF];
    output[2] = '\0';

    print(WHITE, "\n[");
    print(WHITE, args[0]);
    print(WHITE, "] = 0x");
    print(WHITE, output);
    print(WHITE, "\n");
}

void poke_command(void) {
    char args[2][BUFFER_SIZE];
    int count = extract_arguments("poke", args, 2, BUFFER_SIZE);

    if (count < 2) {
        print(WHITE, "\nUsage: poke 0xADDRESS 0xVALUE\n");
        return;
    }

    unsigned long address = parse_hex_address(args[0]);
    unsigned char value = (unsigned char)parse_hex_address(args[1]);

    poke_memory(address, value);

    print(WHITE, "\n[");
    print(WHITE, args[0]);
    print(WHITE, "] <= 0x");

    const char *hex = "0123456789ABCDEF";
    char val_str[3];
    val_str[0] = hex[(value >> 4) & 0xF];
    val_str[1] = hex[value & 0xF];
    val_str[2] = '\0';
    print(WHITE, val_str);
    print(WHITE, "\n");
}

