all: run

run:
	gcc -o main main.c -Wall -lncurses
	./main

build:
	gcc -o main login.c main.c -Wall -lncurses

.PHONY: login run

login:
	gcc -o login login.c -Wall -lncurses
	./login
