#include "keyboard.h"

// Basic US QWERTY Set 1 Scancode to ASCII lookup table
const char scancode_to_ascii[] = {
    0,   27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0,
  '*',    0, ' ',   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,    0,   0,   0, '-',   0,   0,   0, '+',   0,   0,   0,   0,
    0,    0,   0,   0,   0,   0
};

char keyboard_scancode_to_ascii(uint8_t scancode) {
    if (scancode < sizeof(scancode_to_ascii)) {
        return scancode_to_ascii[scancode];
    }
    return 0;
}
