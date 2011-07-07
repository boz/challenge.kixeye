
CFLAGS  := -Wall -g --std=c99
objects := $(patsubst %.c,%.o,$(wildcard *.c))

all: server client

server: server.o reactor.o message.o client_handler.o
	$(CC) $(CFLAGS) -o server $^

client: client.o reactor.o message.o
	$(CC) $(CFLAGS) -o client $^

clean:
	rm $(objects) server client || true
	
