CC = arm-none-eabi-gcc
CFLAGS = -mcpu=cortex-m4 -mthumb -nostartfiles -nostdlib -T linker.ld -g -O0

all:
	mkdir -p build
	$(CC) $(CFLAGS) src/main.c -o build/kiln_ctrl.elf

clean:
	rm -rf build/
