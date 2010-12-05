#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include "../../engine/include/vsx_string.h"

bool app_ctrl = false;
bool app_alt = false;
bool app_shift = false;
bool dual_monitor = false;
bool disable_randomizer = false;
// implementation of app externals
int app_argc = 0;
char** app_argv;

//========================================================================
// main()
//========================================================================

#define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define BUFLEN 256*1024



// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


class vsx_command_list_server
{
  pthread_t         worker_t;
  pthread_attr_t    worker_t_attr;

  int listen_sock;
  int status;
  struct addrinfo hints;
  struct addrinfo *servinfo;  // will point to the results

public:

  static void* server(void *ptr)
  {

    memset(&hints, 0, sizeof hints); // make sure the struct is empty
    hints.ai_family = AF_INET6; //AF_UNSPEC;     // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

    if ((status = getaddrinfo(NULL, "4242", &hints, &servinfo)) != 0)
    {
      fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
      exit(1);
    }

    listen_sock = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if (listen_sock == -1) handle_error("socket");

    int tr=1;

    // kill "Address already in use" error message
    if (setsockopt(listen_sock,SOL_SOCKET,SO_REUSEADDR,&tr,sizeof(int)) == -1)  handle_error("setsockopt");

    if (bind(listen_sock, servinfo->ai_addr, servinfo->ai_addrlen) == -1) handle_error("bind");

    char s[INET6_ADDRSTRLEN];

    freeaddrinfo(servinfo);

    if (listen(listen_sock,5) == -1) handle_error("bind");

    char recv_buf[BUFLEN];


    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    addr_size = sizeof their_addr;
    while (1)
    {
      printf("waiting for connection...\n");
      int recv_sock = accept(
                              listen_sock,
                              (struct sockaddr *)&their_addr,
                              &addr_size
                            );

      inet_ntop(
        their_addr.ss_family,
        get_in_addr((struct sockaddr *)&their_addr),
        s,
        sizeof s
      );
      printf("server: got connection from %s\n", s);
      //
      memset(&recv_buf,0,BUFLEN);
      bool run = true;
      while (run)
      {
        status = recv(recv_sock, &recv_buf, BUFLEN, MSG_DONTWAIT);
        //printf("status: %d\n",status);
        
        vsx_string msga = ".\n";
        send(
              recv_sock,
              (const void*)msga.c_str(),
              msga.size(),
              0
            );
        if (recv_buf[0] != 0)
        {
          printf("got: %s",vsx_string(recv_buf).c_str());
          if (vsx_string(recv_buf).substr(0,4) == "quit")
          {
            vsx_string msg = "bye bye\n";
            send(
                  recv_sock,
                  (const void*)msg.c_str(),
                  msg.size(),
                  0
                );
            printf("closing connection for client\n");
            close(recv_sock);
            run = false;
          }
          memset(&recv_buf,0,BUFLEN);
        }
        usleep(100);
      }
    }
    close(listen_sock);
  }

  void start()
  {
    pthread_attr_init(&worker_t_attr);
    pthread_create(&worker_t, &worker_t_attr, &server, (void*)this);
    pthread_detach(worker_t);
  }
};


int main(int argc, char* argv[])
{
  app_argc = argc;
  vsx_command_list_server my_server;
  my_server.start();
  while (1)
  {
    printf("...\n");
    sleep(1);
  }
  return 0;
}
