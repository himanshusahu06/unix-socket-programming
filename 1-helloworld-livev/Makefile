all: libevserver.out

OPTS = -I../lib/
# lib dependencies
LIBEVENT=/opt/libevent
LIBS = ../lib/error.o ../lib/sock.o
LIBS +=		$(LIBEVENT)/lib/libevent.a
CFLAGS =	-I$(LIBEVENT)/include -Wall -DUSE_DEBUG

libevserver.out: libevserver.c $(LIBS)
	$(CC) $(CFLAGS) $(OPTS) $(LIBS) libevserver.c -o $@

clean:
	$(RM) *.out *.o *.so
