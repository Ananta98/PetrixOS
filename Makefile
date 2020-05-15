GCCPARAMS = -Ikernel/include -Iinclude -std=gnu99 -ffreestanding -O2 -Wall -Wextra
GCC = i686-elf-gcc
ASM = nasm -felf32
GCCLINKING = -ffreestanding -O2 -nostdlib

KERNEL_OBJS = $(patsubst %.c,%.o,$(wildcard kernel/boot/*.c))
KERNEL_OBJS += $(patsubst %.asm,%.o,$(wildcard kernel/boot/*.asm))
KERNEL_OBJS += $(patsubst %.asm,%.o,$(wildcard kernel/arch/*.asm))
KERNEL_OBJS += $(patsubst %.c,%.o,$(wildcard kernel/arch/*.c))
KERNEL_OBJS += $(patsubst %.c,%.o,$(wildcard kernel/mm/*.c))
KERNEL_OBJS += $(patsubst %.c,%.o,$(wildcard kernel/lib/*.c))
KERNEL_OBJS += $(patsubst %.c,%.o,$(wildcard kernel/drivers/*.c))
KERNEL_OBJS += $(patsubst %.c,%.o,$(wildcard kernel/mm/*.c))
KERNEL_OBJS += $(patsubst %.c,%.o,$(wildcard kernel/proc/*.c))

kernel/boot/%.o : kernel/boot/%.c
	$(GCC) -c $< -o $@ $(GCCPARAMS)

kernel/boot/%.o : kernel/boot/%.asm
	$(ASM) $< -o $@

kernel/arch/%.o : kernel/arch/%.c
	$(GCC) -c $< -o $@ $(GCCPARAMS)

kernel/arch/%.o : kernel/arch/%.asm
	$(ASM) $< -o $@

kernel/arch%.o : kernel/%.c
	$(GCC) -c $< -o $@ $(GCCPARAMS)

 kernel/lib/%.o :  kernel/lib/%.c
	$(GCC) -c $< -o $@ $(GCCPARAMS)

kernel/drivers/%.o : kernel/drivers/%.c
	$(GCC) -c $< -o $@ $(GCCPARAMS)

kernel/proc/%.o : kernel/proc/%.c
	$(GCC) -c $< -o $@ $(GCCPARAMS)

kernel/mm/%.o : kernel/mm/%.c
	$(GCC) -c $< -o $@ $(GCCPARAMS)


PetrixOS.bin: kernel/boot/linker.ld  $(KERNEL_OBJS)
	$(GCC) -T $< -o $@ $(GCCLINKING) $(KERNEL_OBJS)

PetrixOS.iso: PetrixOS.bin
	mkdir -p iso/boot/grub
	cp PetrixOS.bin iso/boot/PetrixOS.bin
	echo 'set timeout=0'> iso/boot/grub/grub.cfg
	echo 'set default=0' >> iso/boot/grub/grub.cfg
	echo ''
	echo 'menuentry "PetrixOS" {' >> iso/boot/grub/grub.cfg
	echo '	multiboot /boot/PetrixOS.bin' >> iso/boot/grub/grub.cfg
	echo '  boot' >> iso/boot/grub/grub.cfg
	echo '}' >> iso/boot/grub/grub.cfg
	grub-mkrescue -o PetrixOS.iso iso 
	rm -rf iso
run:
	qemu-system-i386 -cdrom PetrixOS.iso	

clean:
	# rm *\.iso
	# rm *\.bin
	rm kernel/boot/*\.o
	rm kernel/arch/*\.o
	rm kernel/mm/*\.o
	rm kernel/lib/*\.o
	rm kernel/drivers/*\.o
	rm kernel/proc/*\.o