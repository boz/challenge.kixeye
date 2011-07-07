#ifndef KIXEYE_MACROS_H
# define KIXEYE_MACROS_H

#define DEBUG(message, ...) do{ fprintf(stderr,"%s:%s(): ",__FILE__,__func__);\
	   fprintf(stderr, message, ##__VA_ARGS__ ); \
	   fprintf(stderr, "\n"); \
} while(0);

#endif /* KIXEYE_MACROS_H */
