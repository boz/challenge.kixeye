#include<arpa/inet.h>
#include<getopt.h>
#include<string.h>
#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>

#include "reactor.h"
#include "message.h"
#include "macros.h"

static struct {
  int  port;
  char host[255];
  char name[255];
} config = {
  6666 , "127.0.0.1", "kixeye"
};

static void usage(const char * name) {
  fprintf(stderr,"Usage: %s [-p port] [-a address] [-n name]\n",name);
  exit(1);
}

static void do_getopt(int argc, char *argv[]) {
  int c;
  while((c = getopt(argc,argv,"hp:a:n:")) != -1) {
    switch(c) {
     case 'p': config.port = atoi(optarg)                      ; break;
     case 'a': strncpy(config.host,optarg,sizeof(config.host)) ; break;
     case 'n': strncpy(config.name,optarg,sizeof(config.name)) ; break;
     default : usage(*argv)               ; break;
    }
  }
}

static int initialize_socket() {
  int fd;
  struct sockaddr_in addr;

  if((fd = socket(PF_INET,SOCK_STREAM,0)) < 0) {
    perror("socket");
    exit(1);
  }

  addr.sin_family      = AF_INET;
  addr.sin_port        = htons(config.port);
  addr.sin_addr.s_addr = inet_addr(config.host);
  bzero(addr.sin_zero,sizeof(addr.sin_zero));

  if(connect(fd,(struct sockaddr *)&addr,sizeof(addr)) < 0) {
    perror("socket");
    exit(1);
  }

  return fd;
}

static void handle_incoming_message(int fd, void * user) {
  static message_t incoming;
  static size_t    incoming_read;

  int nread = message_read_partial(fd, &incoming, incoming_read);

  if(nread <= 0) {
    printf("connection closed\n");
    exit(0);
  }

  incoming_read += nread;

  message_t * message;
  if((message = message_parse_raw(&incoming,incoming_read))) {
    /* message is complete */
    printf(">>> %s\n",message->buffer);
    incoming_read = 0;
    bzero(&incoming,sizeof(incoming));
  }
}

static void handle_terminal_input(int fd, void * user) {
  int  server_fd = *(int*)user;
  char buffer[128];
  int nread;

  if((nread = read(fd,buffer,sizeof(buffer))) < 0) {
    perror("read");
    exit(1);
  }
  message_write_string(server_fd,mt_broadcast,buffer);
}

int main(int argc, char *argv[]) {
  int fd;

  do_getopt(argc,argv);
  fd = initialize_socket();

  reactor_initialize();
  reactor_add_listener(fd,handle_incoming_message,0);
  reactor_add_listener(STDIN_FILENO,handle_terminal_input,&fd);

  message_write_string(fd,mt_connect,config.name);

  reactor_run();
  exit(0);
}
