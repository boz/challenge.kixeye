#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<getopt.h>
#include<netinet/in.h>
#include<strings.h>
#include<stdbool.h>

#include "macros.h"
#include "reactor.h"
#include "client_handler.h"

static struct {
  int port;
  int backlog;
} config = {
  6666, 10
};

static void usage(const char * name) {
  fprintf(stderr,"Usage: %s [-p port]\n",name);
  exit(1);
}

static void do_getopt(int argc, char *argv[]) {
  int c;
  while((c = getopt(argc,argv,"hp:")) != -1) {
    switch(c) {
     case 'p': config.port = atoi(optarg) ; break;
     default : usage(*argv)               ; break;
    }
  }
}

static int initialize_socket() {

  int fd  ,
      yes = 1;

  struct sockaddr_in addr;

  if((fd = socket(PF_INET,SOCK_STREAM,0)) < 0) {
    perror("socket");
    exit(1);
  }

  if (setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) < 0) {
    perror("setsockopt");
    exit(1);
  }

  addr.sin_family      = AF_INET;
  addr.sin_port        = htons(config.port);
  addr.sin_addr.s_addr = 0;
  bzero(addr.sin_zero,sizeof(addr.sin_zero));

  if(bind(fd,(struct sockaddr *)&addr,sizeof(addr)) < 0) {
    perror("socket");
    exit(1);
  }

  if(listen(fd,config.backlog) < 0) {
    perror("listen");
    exit(1);
  }

  return fd;
}

static void accept_connection(int fd, void * user) {
  int cfd;
  struct sockaddr addr;
  socklen_t addrlen;

  if((cfd = accept(fd,&addr,&addrlen)) < 0) {
    perror("accept");
    return;
  }
  client_handler_create(cfd);
}

int main(int argc, char *argv[]) {
  int fd;

  do_getopt(argc,argv);
  fd = initialize_socket();

  reactor_initialize();
  reactor_add_listener(fd,accept_connection,0);

  reactor_run();
  exit(0);
}
