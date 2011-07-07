#include<netinet/in.h>
#include<strings.h>
#include "reactor.h"

/* TODO: use a fd -> (cb,user) map */
typedef struct reactor_listener {
	int fd;
	reactor_callback cb;
	void * user;
} listener_t;

static struct {
	fd_set readset;
	listener_t listeners[REACTOR_MAX_LISTENERS];
} reactor;

void reactor_initialize() {
	static int reactor_initialized;
	if(!reactor_initialized) {
		FD_ZERO(&reactor.readset);
		reactor_initialized = 1;
	}
}

int reactor_add_listener(int fd, reactor_callback cb, void * user) {
	int idx;
	for(idx = 0; idx < REACTOR_MAX_LISTENERS; idx++) {
		if(!reactor.listeners[idx].fd) {
			reactor.listeners[idx].fd   = fd;
			reactor.listeners[idx].cb   = cb;
			reactor.listeners[idx].user = user;
			FD_SET(fd,&reactor.readset);
			return idx;
		}
	}
	return 0;
}

void reactor_remove_listener(int fd) {
	for(int idx = 0; idx < REACTOR_MAX_LISTENERS; idx++) {
		if(reactor.listeners[idx].fd == fd) {
			bzero(&reactor.listeners[idx],sizeof(listener_t));
			FD_CLR(fd,&reactor.readset);
		}
	}
}

