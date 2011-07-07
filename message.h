#ifndef KIXEYE_MESSAGE_H
# define KIXEYE_MESSAGE_H
#include<unistd.h>

#define MESSAGE_BUFFER_SIZE_MAX 255

typedef enum {
  mt_connect   ,
  mt_broadcast
} message_type_t;

typedef struct {
  size_t         size; /* including size field */
  message_type_t type;
} message_header_t;

typedef struct {
  message_header_t header;
  char             buffer[MESSAGE_BUFFER_SIZE_MAX + 1];
} message_t;

message_t * message_parse_raw(message_t * message);
size_t message_body_size(const message_t * message);
int message_read_partial(int fd, message_t * message, size_t offset);
int message_write(int fd, message_t * message);


#endif /* KIXEYE_MESSAGE_H */
