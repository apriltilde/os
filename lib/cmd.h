#ifndef CMD_H
#define CMD_H

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define CMD_AREA_WIDTH 512
#define CMD_AREA_LINES 10

static const int font_char_width = 8;
static const int font_char_height = 16;

static int in_graphics_mode = 0;

// Command line area starting coordinates (centered)
static const int cmd_x_start = (SCREEN_WIDTH - CMD_AREA_WIDTH) / 2; // 256
static const int cmd_y_start = (SCREEN_HEIGHT / 2) - (CMD_AREA_LINES * font_char_height / 2); // roughly centered vertically

// Cursor position for command input
static int gfx_cursor_x = cmd_x_start;
static int gfx_cursor_y = cmd_y_start;
static int gfx_line_height = 12;


// Initialize the command interface
void cmd_init(void);

// Handle keyboard input for command interface
void cmd_handle_input(void);
void gfxprint(char *str, char *color);

#endif
