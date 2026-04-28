#!/bin/bash

# 1. Clean up old files
rm -rf build gameos.iso
mkdir -p build

# 2. Compile the C++ code
for file in src/*.cpp; do
    filename=$(basename -- "$file")
    g++ -c $file -o build/${filename%.cpp}.o \
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
        -Isrc
done

# 3. Link into the kernel ELF
ld build/*.o -T linker.ld -o build/kernel.elf -nostdlib -static -m elf_x86_64

# 4. Prepare the ISO directory
mkdir -p build/iso_root
if [ -f "build/kernel.elf" ]; then
    cp build/kernel.elf limine.cfg boot/limine-bios.sys boot/limine-bios-cd.bin boot/limine-uefi-cd.bin build/iso_root/
else
    echo "ERROR: kernel.elf was not created. Check compiler errors above."
    exit 1
fi

# 5. Create the ISO
xorriso -as mkisofs -b limine-bios-cd.bin \
        -no-emul-boot -boot-load-size 4 -boot-info-table \
        --efi-boot limine-uefi-cd.bin \
        -efi-boot-part --efi-boot-image --protective-msdos-label \
        build/iso_root -o gameos.iso

# 6. Make it bootable
./boot/limine-deploy gameos.iso

# 7. Package the release
zip -q gameos.zip gameos.iso

echo "-------------------------------"
echo "Build Complete: gameos.iso and gameos.zip ready"
echo "-------------------------------"