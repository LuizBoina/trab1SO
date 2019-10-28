CFLAGS=-std=c99
all: trab_1_so
trab_1_so: lista.o vet.o gsh.o main.o 
	gcc -o trab_1_so lista.o vet.o gsh.o main.o
lista.o: lista.c lista.h
	gcc -o lista.o -c lista.c
vet.o: vet.c vet.h
	gcc -o vet.o -c vet.c
gsh.o: gsh.c gsh.h lista.h vet.h
	gcc -o gsh.o -c gsh.c
main.o: main.c gsh.h lista.h vet.h
	gcc -o main.o -c main.c
clean:
	rm -rf *.o
mrproper: clean
	rm -rf trab_1_so
