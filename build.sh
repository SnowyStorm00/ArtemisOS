#!/bin/bash

# 1. Clean up old files
rm -f *.o kernel.elf gameos.iso

# 2. Compile the C++ code
# Added -mcmodel=kernel to fix the relocation error
for file in *.cpp; do
    g++ -c $file -o ${file%.cpp}.o \
        -ffreestanding \
        -fno-stack-protector \
        -fno-stack-check \
        -fno-lto \
        -fno-pie \
        -fno-pic \
        -m64 \
        -march=x86-64 \
        -mno-red-zone \
        -mgeneral-regs-only \
        -mcmodel=kernel \
        -fno-rtti \
        -fno-exceptions \
        -I.
done

# 3. Link into the kernel ELF
ld *.o -T linker.ld -o kernel.elf -nostdlib -static -m elf_x86_64

# 4. Prepare the ISO directory
mkdir -p iso_root
# Only copy if kernel.elf actually exists to avoid errors
if [ -f "kernel.elf" ]; then
    cp kernel.elf limine.cfg limine-bios.sys limine-bios-cd.bin limine-uefi-cd.bin iso_root/
else
    echo "ERROR: kernel.elf was not created. Check compiler errors above."
    exit 1
fi

# 5. Create the ISO
xorriso -as mkisofs -b limine-bios-cd.bin \
        -no-emul-boot -boot-load-size 4 -boot-info-table \
        --efi-boot limine-uefi-cd.bin \
        -efi-boot-part --efi-boot-image --protective-msdos-label \
        iso_root -o gameos.iso

# 6. Make it bootable
./limine-deploy gameos.iso

echo "-------------------------------"
echo "Build Complete: gameos.iso ready"
echo "-------------------------------"