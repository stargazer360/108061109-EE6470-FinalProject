all : main.c irq.c irq.h bootstrap.S
	riscv32-unknown-elf-gcc main.c irq.c bootstrap.S  -o main -g3 -lm -nostartfiles -march=rv32ima -mabi=ilp32 -Wl,--no-relax
	
sim: all
	$(EE6470)/riscv-vp/vp/build/bin/astral32-mc --intercept-syscalls  main
	
dump-elf: all
	riscv32-unknown-elf-readelf -a main
	
dump-code: all
	riscv32-unknown-elf-objdump -D main
	
dump-comment: all
	objdump -s --section .comment main
	
clean:
	rm -f main
