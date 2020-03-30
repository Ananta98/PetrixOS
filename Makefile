GCCPARAMS = -Ikernel/include -Iinclude -std=gnu99 -ffreestanding -O2 -Wall -Wextra
GCC = i686-elf-gcc
ASM = nasm -felf32
GCCLINKING = -ffreestanding -O2 -nostdlib

KERNEL_OBJS = $(patsubst %.asm,%.o,$(wildcard kernel/arch/x86/cpu/*.asm))
KERNEL_OBJS += $(patsubst %.c,%.o,$(wildcard kernel/arch/x86/mm/*.c))
KERNEL_OBJS += $(patsubst %.c,%.o,$(wildcard kernel/arch/x86/cpu/*.c))
KERNEL_OBJS += $(patsubst %.c,%.o,$(wildcard kernel/arch/x86/device/*.c))
KERNEL_OBJS += $(patsubst %.c,%.o,$(wildcard kernel/mm/*.c))
KERNEL_OBJS += $(patsubst %.c,%.o,$(wildcard kernel/lib/*.c))
KERNEL_OBJS += $(patsubst %.c,%.o,$(wildcard kernel/drivers/*.c))

kernel/%.o : kernel/%.c
	$(GCC) -c $< -o $@ $(GCCPARAMS)

kernel/arch/x86/cpu/%.o : kernel/arch/x86/cpu/%.asm
	$(ASM) $< -o $@

kernel/arch/x86/device/%.o : kernel/arch/x86/device/%.c
	$(GCC) -c $< -o $@ $(GCCPARAMS)

kernel/arch/x86/mm/%.o : kernel/arch/x86/mm/%.c
	$(GCC) -c $< -o $@ $(GCCPARAMS)

 kernel/lib/%.o :  kernel/lib/%.c
	$(GCC) -c $< -o $@ $(GCCPARAMS)

kernel/drivers/%.o : kernel/drivers/%.c
	$(GCC) -c $< -o $@ $(GCCPARAMS)

PetraOS.bin: linker.ld  $(KERNEL_OBJS)
	$(GCC) -T $< -o $@ $(GCCLINKING) $(KERNEL_OBJS)

PetraOS.iso: PetraOS.bin
	mkdir -p iso/boot/grub
	cp PetraOS.bin iso/boot/PetraOS.bin
	echo 'set timeout=0'> iso/boot/grub/grub.cfg
	echo 'set default=0' >> iso/boot/grub/grub.cfg
	echo ''
	echo 'menuentry "PetraOS" {' >> iso/boot/grub/grub.cfg
	echo '	multiboot /boot/PetraOS.bin' >> iso/boot/grub/grub.cfg
	echo '  boot' >> iso/boot/grub/grub.cfg
	echo '}' >> iso/boot/grub/grub.cfg
	grub-mkrescue -o PetraOS.iso iso 
	rm -rf iso
run:
	qemu-system-i386 -cdrom PetraOS.iso	

clean:
	rm kernel/arch/x86/cpu/*\.o
	rm kernel/arch/x86/device/*\.o
	rm kernel/arch/x86/mm/*\.o
	rm kernel/drivers/*\.o
	rm kernel/lib/*\.o
	rm *\.iso
	rm *\.bin
