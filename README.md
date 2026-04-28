# ArtemisOS

A custom hobby operating system built from scratch.

## Features
- **Limine Bootloader**: Modern, 64-bit boot protocol.
- **Serial Debugging**: Logs kernel output directly to `COM1` via a custom `kprintf` implementation.
- **Interrupt Handling**: Custom GDT and IDT with C++ exception handlers for CPU faults (Divide by Zero, Page Faults, etc.).
- **Basic Framebuffer**: Simple graphical rendering and text drawing capability.

## Building and Running
1. Run `./build.sh` to compile the OS and generate the `gameos.iso` image.
2. Boot `gameos.iso` using QEMU or your preferred virtual machine:
   ```bash
   qemu-system-x86_64 -cdrom gameos.iso -serial stdio
   ```
   *Note: Using `-serial stdio` allows you to see the kernel's debug logs in your terminal.*
