all: process multiply matformatter formatter.o

process:  matrix_mult.c formatter.o
	gcc -o matrix_mult matrix_mult.c matrix_formatter.o -I./include

multiply: multiply.c
	gcc -o multiply multiply.c

matformatter: formatter.o
	gcc -o matformatter matformatter.c matrix_formatter.o -I./include

formatter.o: matrix_formatter.c
	gcc -o matrix_formatter.o -c matrix_formatter.c  -I./include

clean:
	rm *.o matformatter mbuf35* matrix_mult multiply
