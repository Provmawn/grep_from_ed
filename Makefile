# -*- Makefile -*- 
all: grep_from_ed

grep_from_ed: grep_from_ed.c func.h
	gcc -g -w -o grep_from_ed grep_from_ed.c

clean:
	rm grep_from_ed
