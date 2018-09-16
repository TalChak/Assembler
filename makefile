
all : assembler

assembler : assembler.c first_pass.c second_pass.c assembler.h
	gcc -Wall -ansi -pedantic assembler.c first_pass.c second_pass.c -o assembler

clean : rm *~
