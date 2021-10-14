

all: main.o
	ld main.o -o main
	rm main.o

main.o: main.asm
	nasm -f elf64 -g -I std/ -w+all -o main.o main.asm
