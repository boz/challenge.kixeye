#include<stdbool.h>
#include<arpa/inet.h>
#include<stdio.h>

#include "message.h"
#include "macros.h"

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
  DEBUG("wrote %d",nwrote);
  return nwrote;
}
