SRC_DIR :=	src
OBJ_DIR :=	obj

SRCS = 		$(wildcard $(SRC_DIR)/*.c)
SRCSS = 	$(wildcard $(SRC_DIR)/*.S)
OBJS = 		$(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
OBJSS =		$(SRCSS:$(SRC_DIR)/%.S=$(OBJ_DIR)/%.o)
CPPFLAGS := 	-Iinclude -MMD -MP -static -fno-inline
CFLAGS = 	-Wall -O3 -mgeneral-regs-only -fno-lto -fno-builtin 
LDFLAGS = 	-nostdlib 

CC = /home/elvis/Downloads/arm-gnu-toolchain-12.2.rel1-x86_64-aarch64-none-elf/bin/aarch64-none-elf-gcc
LD = /home/elvis/Downloads/arm-gnu-toolchain-12.2.rel1-x86_64-aarch64-none-elf/bin/aarch64-none-elf-ld
OBJCOPY = /home/elvis/Downloads/arm-gnu-toolchain-12.2.rel1-x86_64-aarch64-none-elf/bin/aarch64-none-elf-objcopy
QEMU = qemu-system-aarch64 -drive if=sd,file=/home/elvis/course/osc2023/lab8/sfn_nctuos.img,format=raw


all: clean kernel8.img initramfs.cpio

#initramfs.cpio: initramfs
#	cd initramfs; find . -print -depth | cpio -H newc -o > $@; mv $@ ..

initramfs.cpio: vfs2.img
	ls $^ | cpio -H newc -o > $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.S
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

start.o: start.S 
	$(CC) $(CFLAGS) -c -o $@ $<

boot.o: boot.S | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

user.o: user.S
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

kernel8.elf:  start.o $(OBJS) $(OBJSS)
	$(LD) $(LDFLAGS) $^ -T linker.ld -o $@

user.elf: user.o
	$(LD) $(LDFLAGS) $< -T user_program.ld -o $@

%.img: %.elf
	$(OBJCOPY) -O binary $< $@ 

run_asm: kernel8.img initramfs.cpio
	$(QEMU) -M raspi3 -display none -serial null -serial stdio -d in_asm -initrd initramfs.cpio -dtb bcm2710-rpi-3-b-plus.dtb -kernel $<

run_int: kernel8.img initramfs.cpio
	$(QEMU) -M raspi3 -display none -serial null -serial stdio -d int -initrd initramfs.cpio -dtb bcm2710-rpi-3-b-plus.dtb -kernel $<

run_dbg: kernel8.img initramfs.cpio
	$(QEMU) -M raspi3 -display none -serial null -serial stdio -S -s -initrd initramfs.cpio -dtb bcm2710-rpi-3-b-plus.dtb -kernel $<

run: kernel8.img initramfs.cpio 
	$(QEMU) -M raspi3 -serial null -serial stdio -initrd initramfs.cpio -dtb bcm2710-rpi-3-b-plus.dtb -kernel $<

run-nom: kernel8.img initramfs.cpio 
	$(QEMU) -M raspi3 -display none -serial null -serial stdio -initrd initramfs.cpio -dtb bcm2710-rpi-3-b-plus.dtb -kernel $<
$(OBJ_DIR):
	mkdir -p $@

clean:
	@rm kernel8.img *.o *.d *.elf >/dev/null 2>/dev/null || true
	@rm -r $(OBJ_DIR) >/dev/null 2>/dev/null || true

-include $(OBJS:.o=.d)	
