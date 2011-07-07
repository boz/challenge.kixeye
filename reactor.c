#include<netinet/in.h>
#include<strings.h>
#include<sys/select.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include "reactor.h"
#include "macros.h"

#define REACTOR_MAX_LISTENERS 256

/* TODO: use a fd -> (cb,user) map */
typedef struct reactor_listener {
	int fd;
	reactor_callback cb;
	void * user;
} listener_t;

static struct {
  int        running;
	fd_set     readset;
  int        maxfd;
	listener_t listeners[REACTOR_MAX_LISTENERS];
} reactor;

#define FOR_EACH_LISTENER(var) \
  for( listener_t * (var) = &reactor.listeners[0];  \
       ((var) - &reactor.listeners[0]) < REACTOR_MAX_LISTENERS; \
       (var)++)

void reactor_initialize() {
	static bool reactor_initialized = false;
	if(!reactor_initialized) {
		FD_ZERO(&reactor.readset);
		reactor_initialized = true;
	}
}

bool reactor_add_listener(int fd, reactor_callback cb, void * user) {
  FOR_EACH_LISTENER(listener) {
    if(!listener->fd) {
			listener->fd   = fd;
			listener->cb   = cb;
			listener->user = user;
			FD_SET(fd,&reactor.readset);
      if(reactor.maxfd < fd)
        reactor.maxfd = fd;
			return true;
    }
  }
	return false;
}

static void recalculate_maxfd() {
  int max = 0;
  FOR_EACH_LISTENER(listener) {
    if(listener->fd > max)
      max = listener->fd;
  }
  reactor.maxfd = max;
}

void reactor_remove_listener(int fd) {
  FOR_EACH_LISTENER(listener) {
    if(listener->fd == fd) {
			DEBUG("removing fd %d",fd);
			bzero(listener,sizeof(listener_t));
			FD_CLR(fd,&reactor.readset);
      if(reactor.maxfd == fd)
        recalculate_maxfd();
      break;
    }
  }
}

void reactor_run() {
  reactor.running = true;
  while(reactor.running) {
    fd_set readset = reactor.readset;
    if(select(reactor.maxfd + 1,&readset,0,0,0) < 0) {
      perror("select");
      exit(1);
    }
    FOR_EACH_LISTENER(listener) {
      if(!reactor.running) break;
      if(listener->cb && FD_ISSET(listener->fd,&readset)) {
        listener->cb(listener->fd,listener->user);
      }
    }
  }
}

void reactor_stop() {
  reactor.running = false;
}
