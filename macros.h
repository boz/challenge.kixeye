#ifndef KIXEYE_MACROS_H
# define KIXEYE_MACROS_H

#ifndef TRUE
# define TRUE 1
#endif

#ifndef FALSE
# define FALSE 0
#endif

#define assert_syscall(x) do  {\
  if((x) < 0) { \
    perror(0); \
    exit(1);\
  } \
} while(0);

#endif /* KIXEYE_MACROS_H */
