#include<stdbool.h>
#include "message.h"
message_t * message_parse_raw(message_t * message) {
  size_t size = message_body_size(message);
  if(size > sizeof(message->buffer) - 1)
    size = 0;
  message->buffer[size] = 0;
  return message;
}

size_t message_body_size(const message_t * message) {
  return message->header.size - sizeof(message->header.size);
}

