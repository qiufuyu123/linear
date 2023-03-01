SRC = main.c linear/mem.c linear/linear.c linear/jit-i386.c
OUT = test
.PHONY: all asm
all:$(SRC)
	gcc $(SRC) -o $(OUT) -g  -m32
	./$(OUT)
asm:$(./asm/test.asm)
	nasm ./asm/test.asm -o bins -f elf
	objdump -s -d ./bins
clean:
	rm ./bins
	rm ./test