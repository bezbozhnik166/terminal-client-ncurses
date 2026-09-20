all: run

server:
	python3 ./python_code/server.py

run:
	gcc -o main main.c -Wall -lncurses
	./main

wlogin:
	gcc -o main main.c login.c -Wall -lncurses
	./main

build:
	gcc -o main login.c main.c -Wall -lncurses

.PHONY: login run server

login:
	gcc -o login login.c -Wall -lncurses
	./login
