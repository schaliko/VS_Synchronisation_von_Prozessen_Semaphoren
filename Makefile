all: fork1

fork: main.o
	gcc -o fork1 main.o

main.o: main.c
	gcc -c main.c

clean:
	rm -f fork1 *.o
