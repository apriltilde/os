#ifndef CMD_H
#define CMD_H

// Initialize the command interface
void cmd_init(void);

// Handle keyboard input for command interface
void cmd_handle_input(void);
void gfxprint(char *str, char *color);

#endif
