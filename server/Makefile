CC=gcc
CFLAGS= -pthread -g
SRC=src

server: out $(SRC)/server.c
	$(CC) $(CFLAGS) -I. -o server $(SRC)/server.c

.PHONY: out clean

out:
	mkdir -p output
	
clean:
	rm -f server
	rm -rf output