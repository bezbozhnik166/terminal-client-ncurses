all: run

run:
	gcc -o main main.c -Wall -lncurses
	./main

.PHONY: login run

login:
	gcc -o login login.c -Wall -lncurses
	./login
