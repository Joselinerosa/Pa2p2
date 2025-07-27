CC=gcc
CFLAGS=-pthread -g

client: out src/client.c
	$(CC) $(CFLAGS) -I. -o client src/client.c

.PHONY: out clean

out:
	mkdir -p output
	
clean:
	rm -f client
	rm -rf output