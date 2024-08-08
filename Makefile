# Automating the build process
# make all is called in build.sh script

FILES = ./build/kernel.asm.o

# writes the binaries into os.bin file
all: ./bin/boot.bin ./bin/kernel.bin
	rm -rf ./bin/os.bin
	dd if=./bin/boot.bin >> ./bin/os.bin
	dd if=./bin/kernel.bin >> ./bin/os.bin
	dd if=/dev/zero bs=512 count=100 >> ./bin/os.bin

# assembles the binary files
./bin/boot.bin: ./src/boot/boot.asm
	nasm -f bin ./src/boot/boot.asm -o ./bin/boot.bin

./build/kernel.asm.o: ./src/kernel.asm
	nasm -f elf -g ./src/kernel.asm -o ./build/kernel.asm.o

# links the object files and generates the kernel.bin
# (-g enables debugging symbols)
# (-T to specify linker script, -ffreestanding to not depend on the standard library, 
# -O0 optimization off, -nostdlib to not use the standard system startup files or libraries when linking)
./bin/kernel.bin: $(FILES)
	i686-elf-ld -g -relocatable $(FILES) -o ./build/kernelfull.o
	i686-elf-gcc -T ./src/linker.ld -o ./bin/kernel.bin -ffreestanding -O0 -nostdlib ./build/kernelfull.o

# removes the binary and object files when called
clean:
	rm -rf ./bin/*.bin
	rm -rf ./build/*.o