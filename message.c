#include<stdbool.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<string.h>

#include "message.h"
#include "macros.h"

message_t * message_parse_raw(message_t * message, size_t nread) {
  if(message->header.size != nread) {
    return 0;
  }
  size_t size = message_body_size(message);
  if(size > sizeof(message->buffer) - 1)
    size = 0;
  message->buffer[size] = 0;
  return message;
}

size_t message_body_size(const message_t * message) {
  return message->header.size - sizeof(message->header.size);
}

int message_read_partial(int fd, message_t * message, size_t offset) {
  size_t nleft, nread;

  if(offset < sizeof(message->header)) {
    /* haven't read the length yet */
    nleft = sizeof(message->header) - offset;
  } else {
    /* reading the message buffer */
    nleft = message->header.size - offset;
  }

  nread = read(fd,((char*)message) + offset, nleft);
  if(nread <= 0) {
    return nread;
  }

  /* more to read */
  if(nread != nleft) {
    return nread;
  }

  offset += nread;

  /* read the length */
  if(offset == sizeof(message->header)) {
    message->header.size = ntohl(message->header.size);
    message->header.type = ntohl(message->header.type);

    /* validate length */
    if(message->header.size  > sizeof(*message) ||
       message->header.size <= sizeof(message->header.size)) {
      return -1;
    }
  }
  return nread;
}

int message_write(int fd, message_t * message) {
  size_t sz = message->header.size;
  message->header.size = htonl(message->header.size);
  message->header.type = htonl(message->header.type);
  int nwrote = write(fd,message,sz);
  return nwrote;
}

void message_construct_string(message_t * message, message_type_t type, const char * buffer) {
  size_t length = strlen(buffer);
  if(length > sizeof(message->buffer) - 1)
    length = sizeof(message->buffer) - 1;
  strncpy(message->buffer,buffer,length);
  message->header.type = type;
  message->header.size = sizeof(message->header) + length;
}

int message_write_string(int fd, message_type_t type, const char * buffer) {
  message_t message;
  message_construct_string(&message,type,buffer);
  return message_write(fd,&message);
}
