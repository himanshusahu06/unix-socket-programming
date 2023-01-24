all: helloworldserver.out helloworldclient.out

OPTS = -I../lib/
# lib dependencies
LIBS = ../lib/error.o ../lib/sock.o

helloworldserver.out: helloworldserver.c $(LIBS)
	$(CC) $(OPTS) $(LIBS) helloworldserver.c -o $@

helloworldclient.out: helloworldclient.c $(LIBS)
	$(CC) $(OPTS) $(LIBS) helloworldclient.c -o $@

clean:
	$(RM) *.out *.o *.so
