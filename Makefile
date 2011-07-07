
CFLAGS  := -Wall -g --std=c99
objects := $(patsubst %.c,%.o,$(wildcard *.c))

server: $(objects)
	$(CC) $(CFLAGS) -o server $(objects)

clean:
	rm $(objects) server || true
	
