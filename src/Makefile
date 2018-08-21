# Builds regen and cleans up the .o files

all: main.o utils.o rdparser.o strgen.o
	gcc main.o utils.o rdparser.o strgen.o -o regen
	rm *.o
objects: main.o utils.o rdparser.o strgen.o
main.o:
	gcc main.c -c -o main.o
utils.o:
	gcc utils.c -c -o utils.o
rdparser.o:
	gcc rdparser.c -c -o rdparser.o
strgen.o:
	gcc strgen.c -c -o strgen.o
clean:
	rm regen
	rm *.o