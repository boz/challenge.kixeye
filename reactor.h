#ifndef KIXEYE_REACTOR_H
# define KIXEYE_REACTOR_H

#define REACTOR_MAX_LISTENERS 256

typedef void (*reactor_callback)(int fd, void * user);

void reactor_initialize();
int reactor_add_listener(int fd, reactor_callback cb, void * user);
#endif /* KIXEYE_REACTOR_H */
