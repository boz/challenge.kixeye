#ifndef KIXEYE_REACTOR_H
# define KIXEYE_REACTOR_H

typedef void (*reactor_callback)(int fd, void * user);

void reactor_initialize();

void reactor_run();
void reactor_stop();

bool reactor_add_listener(int fd, reactor_callback cb, void * user);
void reactor_remove_listener(int fd);
#endif /* KIXEYE_REACTOR_H */
