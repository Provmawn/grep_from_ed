# -*- Makefile -*- 
all: main

main: main.c func.h
	gcc -g -w -o main main.c func.h

clean:
	rm main
