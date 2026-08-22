CC = gcc
LD = ld

CFLAGS = -Wall -Wextra -O2 -pipe \
         -ffreestanding \
         -fno-pic \
         -fno-pie \
         -fno-stack-protector \
         -mno-red-zone \
         -mcmodel=kernel \
         -Iinclude

LDFLAGS = -T linker.ld -nostdlib -z max-page-size=0x1000

KERNEL = build/kernel.elf
ISO = build/myos.iso
LIMINE = limine/limine

SOURCES = \
	src/kernel/main.c \
	src/kernel/boot/limine.c \
	src/kernel/drivers/keyboard.c \
	src/kernel/drivers/serial.c \
	src/kernel/graphics/framebuffer.c \
	src/kernel/terminal/terminal.c \
	src/kernel/time/delay.c

OBJECTS = \
	build/main.o \
	build/limine.o \
	build/keyboard.o \
	build/serial.o \
	build/framebuffer.o \
	build/terminal.o \
	build/delay.o

all: $(ISO)

build:
	mkdir -p build

$(LIMINE):
	$(MAKE) -C limine limine
	chmod +x $(LIMINE)

build/main.o: src/kernel/main.c | build
	$(CC) $(CFLAGS) -c src/kernel/main.c -o build/main.o

build/limine.o: src/kernel/boot/limine.c | build
	$(CC) $(CFLAGS) -c src/kernel/boot/limine.c -o build/limine.o

build/keyboard.o: src/kernel/drivers/keyboard.c | build
	$(CC) $(CFLAGS) -c src/kernel/drivers/keyboard.c -o build/keyboard.o

build/serial.o: src/kernel/drivers/serial.c | build
	$(CC) $(CFLAGS) -c src/kernel/drivers/serial.c -o build/serial.o

build/framebuffer.o: src/kernel/graphics/framebuffer.c | build
	$(CC) $(CFLAGS) -c src/kernel/graphics/framebuffer.c -o build/framebuffer.o

build/terminal.o: src/kernel/terminal/terminal.c | build
	$(CC) $(CFLAGS) -c src/kernel/terminal/terminal.c -o build/terminal.o

build/delay.o: src/kernel/time/delay.c | build
	$(CC) $(CFLAGS) -c src/kernel/time/delay.c -o build/delay.o

$(KERNEL): $(OBJECTS) linker.ld
	$(LD) $(LDFLAGS) $(OBJECTS) -o $(KERNEL)

$(ISO): $(KERNEL) $(LIMINE)
	mkdir -p build/iso/EFI/BOOT

	cp $(KERNEL) build/iso/
	cp limine.conf build/iso/

	cp limine/limine-bios.sys \
	   limine/limine-bios-cd.bin \
	   limine/limine-uefi-cd.bin \
	   build/iso/

	cp limine/BOOTX64.EFI \
	   limine/BOOTIA32.EFI \
	   build/iso/EFI/BOOT/

	xorriso -as mkisofs \
		-b limine-bios-cd.bin \
		-no-emul-boot \
		-boot-load-size 4 \
		-boot-info-table \
		--efi-boot limine-uefi-cd.bin \
		-efi-boot-part \
		--efi-boot-image \
		--protective-msdos-label \
		build/iso \
		-o $(ISO)

	./$(LIMINE) bios-install $(ISO)

run: $(ISO)
	qemu-system-x86_64 -cdrom $(ISO) -m 2G

clean:
	rm -rf build
	$(MAKE) -C limine clean