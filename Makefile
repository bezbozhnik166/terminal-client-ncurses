all: run

run:
	gcc -o main main.c -Wall -lncurses
	./main
