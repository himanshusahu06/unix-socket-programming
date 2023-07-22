all: simpleserver.out simpleserver.out

OPTS = -I../lib/
# lib dependencies
LIBS = ../lib/error.o ../lib/sock.o

simpleserver.out: simpleserver.c $(LIBS)
	$(CC) $(OPTS) $(LIBS) simpleserver.c -o $@

clean:
	$(RM) *.out *.o *.so
