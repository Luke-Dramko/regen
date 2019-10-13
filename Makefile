# Builds regen.  Default commands cleans up .o files, too.
# Author Luke Dramko
srcdir=src/

all: $(srcdir)main.o $(srcdir)utils.o $(srcdir)rdparser.o $(srcdir)strgen.o
	gcc $(srcdir)main.o $(srcdir)utils.o $(srcdir)rdparser.o $(srcdir)strgen.o -o regen
	$(RM) $(srcdir)*.o
objects: $(srcdir)main.o $(srcdir)utils.o $(srcdir)rdparser.o $(srcdir)strgen.o
main.o:
	gcc $(srcdir)main.c -c -o $(srcdir)main.o
utils.o:
	gcc $(srcdir)utils.c -c -o $(srcdir)utils.o
rdparser.o:
	gcc $(srcdir)rdparser.c -c -o $(srcdir)rdparser.o
strgen.o:
	gcc $(srcdir)strgen.c -c -o $(srcdir)strgen.o
clean:
	$(RM) regen
	$(RM) $(srcdir)*.o
