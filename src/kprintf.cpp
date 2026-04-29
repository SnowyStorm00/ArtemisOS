#include "kprintf.h"
#include "serial.h"
#include <stdint.h>

static void print_int(int num) {
    if (num == 0) {
        serial_write_char('0');
        return;
    }
    if (num < 0) {
        serial_write_char('-');
        num = -num;
    }
    char buf[16];
    int i = 0;
    while (num > 0) {
        buf[i++] = (num % 10) + '0';
        num /= 10;
    }
    while (i > 0) {
        serial_write_char(buf[--i]);
    }
}

static void print_hex(uint64_t num) {
    if (num == 0) {
        serial_write_char('0');
        return;
    }
    char buf[16];
    int i = 0;
    while (num > 0) {
        int rem = num % 16;
        if (rem < 10) {
            buf[i++] = rem + '0';
        } else {
            buf[i++] = rem - 10 + 'a';
        }
        num /= 16;
    }
    while (i > 0) {
        serial_write_char(buf[--i]);
    }
}

void kprintf(const char* format, ...) {
    va_list args;
    va_start(args, format);

    for (int i = 0; format[i] != '\0'; i++) {
        if (format[i] == '%' && format[i + 1] != '\0') {
            i++;
            switch (format[i]) {
                case 'c': {
                    char c = (char)va_arg(args, int);
                    serial_write_char(c);
                    break;
                }
                case 's': {
                    const char* s = va_arg(args, const char*);
                    serial_write_string(s ? s : "(null)");
                    break;
                }
                case 'd': {
                    int num = va_arg(args, int);
                    print_int(num);
                    break;
                }
                case 'x': {
                    uint64_t num = va_arg(args, uint64_t);
                    print_hex(num);
                    break;
                }
                case '%': {
                    serial_write_char('%');
                    break;
                }
                default:
                    serial_write_char('%');
                    serial_write_char(format[i]);
                    break;
            }
        } else {
            serial_write_char(format[i]);
        }
    }

    va_end(args);
}
