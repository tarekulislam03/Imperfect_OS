CC = gcc
LD = ld

# These flags tell GCC "Do not use standard libraries, this is a bare-metal OS"
CFLAGS = -Wall -Wextra -O2 -pipe -ffreestanding -fno-pic -fno-pie -fno-stack-protector -mno-red-zone -mcmodel=kernel -Iinclude
LDFLAGS = -T linker.ld -nostdlib -z max-page-size=0x1000

KERNEL = build/kernel.elf
ISO = build/myos.iso

all: $(ISO)

$(KERNEL): kernel/main.c
	mkdir -p build
	$(CC) $(CFLAGS) -c kernel/main.c -o build/main.o
	$(LD) $(LDFLAGS) build/main.o -o $(KERNEL)

$(ISO): $(KERNEL)
	mkdir -p build/iso/EFI/BOOT
	cp $(KERNEL) build/iso/
	cp limine.conf build/iso/
	cp limine/limine-bios.sys limine/limine-bios-cd.bin limine/limine-uefi-cd.bin build/iso/
	cp limine/BOOTX64.EFI limine/BOOTIA32.EFI build/iso/EFI/BOOT/
	xorriso -as mkisofs -b limine-bios-cd.bin -no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot limine-uefi-cd.bin -efi-boot-part --efi-boot-image --protective-msdos-label \
		build/iso -o $(ISO)
	./limine/limine bios-install $(ISO)

run: $(ISO)
	qemu-system-x86_64 -cdrom $(ISO) -m 2G

clean:
	rm -rf build