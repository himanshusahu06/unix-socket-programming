all: helloworldserver.out

OPTS = -I../lib/
# lib dependencies
LIBEVENT=/opt/libevent
LIBS = ../lib/error.o ../lib/sock.o
LIBS +=		$(LIBEVENT)/lib/libevent.a
CFLAGS =	-I$(LIBEVENT)/include -Wall -DUSE_DEBUG

helloworldserver.out: helloworldserver.c $(LIBS)
	$(CC) $(CFLAGS) $(OPTS) $(LIBS) helloworldserver.c -o $@

clean:
	$(RM) *.out *.o *.so
