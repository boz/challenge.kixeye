#ifndef KIXEYE_MACROS_H
# define KIXEYE_MACROS_H

#define assert_syscall(x) do  {\
  if((x) < 0) { \
    perror(0); \
    exit(1);\
  } \
} while(0);

#endif /* KIXEYE_MACROS_H */
