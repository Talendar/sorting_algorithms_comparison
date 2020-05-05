run:
	./program

all: clean main.o sorting_algorithms.o
	gcc main.o sorting_algorithms.o -o program -lm

main.o: main.c
	gcc -c main.c

sorting_algorithms.o: sorting_algorithms.h sorting_algorithms.h
	gcc -c sorting_algorithms.c

clean:
	rm -rf *.o program
