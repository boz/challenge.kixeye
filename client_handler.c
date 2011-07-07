#include<stdbool.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<stdio.h>

#include "client_handler.h"
#include "reactor.h"
#include "message.h"
#include "macros.h"

#define MAX_CLIENTS      32

typedef enum {
  ss_unused  = 0,
  ss_connecting ,
  ss_connected 
} session_state_t;

typedef struct {
  int             fd;
  session_state_t state;
  char            name[MESSAGE_BUFFER_SIZE_MAX + 1];
  message_t       incoming;
  size_t          incoming_read;
} client_state_t;

static client_state_t clients[MAX_CLIENTS];

#define FOR_EACH_CLIENT(var) \
  for( client_state_t * (var) = &clients[0];  \
       ((var) - &clients[0]) < MAX_CLIENTS; \
       (var)++)

typedef void (*message_handler_t)(client_state_t * client, message_t * message);

typedef struct {
  session_state_t   state;
  message_type_t    type;
  message_handler_t cb;
} client_handler_dispatch_t;

static void connecting_on_connect(client_state_t  * client, message_t * message);
static void connected_on_broadcast(client_state_t * client, message_t * message);

static client_handler_dispatch_t handlers[3] = {
  { ss_connecting, mt_connect  , connecting_on_connect  },
  { ss_connected , mt_broadcast, connected_on_broadcast },
  { 0            , 0           , 0                      }
};

#define FOR_EACH_HANDLER(var) \
  for( client_handler_dispatch_t * (var) = &handlers[0];  \
       (var)->cb; \
       (var)++)

static void connecting_on_connect(client_state_t * client, message_t * message) {
  size_t len = strlen(message->buffer);
  if(len > sizeof(client->name) - 1)
    len = sizeof(client->name) - 1;
  strncpy(client->name,message->buffer,len);
  client->name[len] = 0;
  client->state = ss_connected;
}

static void connected_on_broadcast(client_state_t * client, message_t * message) {
}

static void client_handler_apply_message(client_state_t * client, message_t * message) {
  FOR_EACH_HANDLER(handler) {
    if(handler->state == client->state && handler->type == message->header.type) {
      handler->cb(client,message);
      return;
    }
  }
}

static void client_handler_remove_client(client_state_t * client) {
  DEBUG("removing client handler");
  close(client->fd);
  reactor_remove_listener(client->fd);
  bzero(client, sizeof(*client));
}

static void handle_incoming_data(int fd, void * data) {
  client_state_t * client = data;

  int nread = message_read_partial(fd, &client->incoming, client->incoming_read);

  DEBUG("fd %d nread %d iread %lu msg %s",fd,nread,client->incoming_read,client->incoming.buffer);

  if(nread <= 0) {
    client_handler_remove_client(client);
    return;
  }

  client->incoming_read += nread;

  if(client->incoming.header.size == client->incoming_read) {
    /* message is complete */
    message_t * message = message_parse_raw(&client->incoming);
    client_handler_apply_message(client,message);
    client->incoming_read = 0;
    bzero(&client->incoming,sizeof(client->incoming));
  }
}

bool client_handler_create(int fd) {
  FOR_EACH_CLIENT(client) {
    if(client->state == ss_unused) {
      client->fd      = fd;
      client->state   = ss_connecting;
      client->name[0] = 0;
      client->incoming_read = 0;
      reactor_add_listener(fd,handle_incoming_data,client);
      return true;
    }
  }
  close(fd);
  return false;
}

