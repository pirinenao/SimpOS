# Makefile for building the OS

FILES = ./build/kernel.asm.o ./build/kernel.o ./build/idt/idt.asm.o ./build/idt/idt.o ./build/memory/memory.o ./build/io/io.asm.o ./build/memory/heap/heap.o ./build/memory/heap/kernel_heap.o ./build/memory/paging/paging.o ./build/memory/paging/paging.asm.o ./build/disk/disk.o
INCLUDES = -I./src
FLAGS = -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc

# writes the binaries into os.bin file
all: ./bin/boot.bin ./bin/kernel.bin
	rm -rf ./bin/os.bin
	dd if=./bin/boot.bin >> ./bin/os.bin
	dd if=./bin/kernel.bin >> ./bin/os.bin
	dd if=/dev/zero bs=512 count=100 >> ./bin/os.bin

# assembles the boot.asm into an binary file
./bin/boot.bin: ./src/boot/boot.asm
	nasm -f bin ./src/boot/boot.asm -o ./bin/boot.bin

# assembles the kernel.asm into an object file
./build/kernel.asm.o: ./src/kernel.asm
	nasm -f elf -g ./src/kernel.asm -o ./build/kernel.asm.o

# links the object files and generates the kernel.bin
# (-g enables debugging symbols)
# (-T to specify linker script, -ffreestanding to not depend on the standard library, 
./bin/kernel.bin: $(FILES)
	i686-elf-ld -g -relocatable $(FILES) -o ./build/kernelfull.o
	i686-elf-gcc $(FLAGS) -T ./src/linker.ld -o ./bin/kernel.bin ./build/kernelfull.o

# compiles kernel.c into an object file
./build/kernel.o: ./src/kernel.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/kernel.c -o ./build/kernel.o

# assembles idt.asm into an object file
./build/idt/idt.asm.o: ./src/idt/idt.asm
	nasm -f elf -g ./src/idt/idt.asm -o ./build/idt/idt.asm.o

# compiles idt.c into an object file
./build/idt/idt.o: ./src/idt/idt.c
	i686-elf-gcc $(INCLUDES) -I./src/idt $(FLAGS) -std=gnu99 -c ./src/idt/idt.c -o ./build/idt/idt.o

# compiles memory.c into an object file
./build/memory/memory.o: ./src/memory/memory.c
	i686-elf-gcc $(INCLUDES) -I./src/memory $(FLAGS) -std=gnu99 -c ./src/memory/memory.c -o ./build/memory/memory.o

# assembles io.asm into an object file
./build/io/io.asm.o: ./src/io/io.asm
	nasm -f elf -g ./src/io/io.asm -o ./build/io/io.asm.o

# compiles heap.c into an object file
./build/memory/heap/heap.o: ./src/memory/heap/heap.c
	i686-elf-gcc $(INCLUDES) -I./src/memory/heap $(FLAGS) -std=gnu99 -c ./src/memory/heap/heap.c -o ./build/memory/heap/heap.o

# compiles kernel_heap.c into an object file
./build/memory/heap/kernel_heap.o: ./src/memory/heap/kernel_heap.c
	i686-elf-gcc $(INCLUDES) -I./src/memory/heap $(FLAGS) -std=gnu99 -c ./src/memory/heap/kernel_heap.c -o ./build/memory/heap/kernel_heap.o

# compiles paging.c into an object file
./build/memory/paging/paging.o: ./src/memory/paging/paging.c
	i686-elf-gcc $(INCLUDES) -I./src/memory/paging $(FLAGS) -std=gnu99 -c ./src/memory/paging/paging.c -o ./build/memory/paging/paging.o

# assembles paging.asm into an object file
./build/memory/paging/paging.asm.o: ./src/memory/paging/paging.asm
	nasm -f elf -g ./src/memory/paging/paging.asm -o ./build/memory/paging/paging.asm.o

# compiles paging.c into an object file
./build/disk/disk.o: ./src/disk/disk.c
	i686-elf-gcc $(INCLUDES) -I./src/disk $(FLAGS) -std=gnu99 -c ./src/disk/disk.c -o ./build/disk/disk.o

# clean up generated files
clean:
	rm -rf ./bin/*.bin
	rm -rf ./build/*.o
	rm -rf ./build/idt/*.o
	rm -rf ./build/io/*.o
	rm -rf ./build/memory/*.o
	rm -rf ./build/memory/heap/*.o
	rm -rf ./build/memory/paging/*.o
	rm -rf ./build/disk/*.o





