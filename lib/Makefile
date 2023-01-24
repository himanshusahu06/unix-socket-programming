all: error.o sock.o

error.o: error.c
	$(CC) -o $@ -c error.c

sock.o: sock.c
	$(CC) -o $@ -c sock.c

clean:
	$(RM) *.out *.o *.so
