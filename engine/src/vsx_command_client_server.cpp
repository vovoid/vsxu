/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies Copyright (C) 2003-2011
* @see The GNU Public License (GPL)
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/


#include <vsx_platform.h>
#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
#include "vsx_command_client_server.h"


#include <netinet/in.h>
#include <netinet/tcp.h>

#include <vsx_avector.h>
#include <vsxfst.h>
//#define TCP_NODELAY 1

#define handle_error(msg) \
           perror(msg); \
           return 0;

#define BUFLEN 256*1024


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

vsx_command_list_server::vsx_command_list_server()
{
  cmd_in = cmd_out = 0;
}

// ****************************************************************************
// ****************************************************************************
// VSX_COMMAND_LIST_SERVER ****************************************************
// ****************************************************************************
// ****************************************************************************

void vsx_command_list_server::set_command_lists(vsx_command_list* new_in,
                                                vsx_command_list* new_out)
{
  cmd_in = new_in;
  cmd_out = new_out;
}

bool vsx_command_list_server::start()
{
  if (!cmd_in || !cmd_out) return false;
  pthread_attr_init(&worker_t_attr);
  pthread_create(&worker_t, &worker_t_attr, &server_worker, (void*)this);
  pthread_detach(worker_t);
  return true;
}

void* vsx_command_list_server::server_worker(void *ptr)
{
  printf("server starting...\n");
  vsx_command_list_server* this_ = (vsx_command_list_server*)ptr;
  int listen_sock = -1;
  int status;
  struct addrinfo hints;
  struct addrinfo *servinfo;  // will point to the results
  struct sockaddr_storage their_addr;
  char recv_buf[BUFLEN];
  socklen_t addr_size;
  int tr=1;
  char s[INET6_ADDRSTRLEN];
  bool run = true;
  ssize_t size_recv;
  int keepalive_timer = 0;
  vsx_string message_stack;
  
  memset(&hints, 0, sizeof hints); // make sure the struct is empty
  hints.ai_family = AF_INET; //AF_INET6 or AF_UNSPEC
  hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
  hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

  printf("a\n");

  if ((status = getaddrinfo(NULL, "11030", &hints, &servinfo)) != 0)
  {
    printf("getaddrinfo error: %s\n", gai_strerror(status));
    exit(1);
  }
  printf("b\n");
  listen_sock = socket(
    servinfo->ai_family,
    servinfo->ai_socktype,
    servinfo->ai_protocol);
  if (listen_sock == -1) {
    printf("error in socket\n\n");
    handle_error("socket");
  }
  printf("c socket id: %d\n",listen_sock);


  // kill "Address already in use" error message
  if (setsockopt(listen_sock,SOL_SOCKET,SO_REUSEADDR,&tr,sizeof(int)) == -1)
  {
    printf("error in setsockopt\n");
    handle_error("setsockopt\n");
  }
  printf("d\n");

  if (bind(listen_sock, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
  {
    printf("error in bind\n");
    handle_error("bind");
  }
  printf("e\n");

  freeaddrinfo(servinfo);
  printf("f\n");

  if (listen(listen_sock,5) == -1) {
    printf("error listen\n");
    handle_error("bind");
  }
  printf("g\n");

  addr_size = sizeof their_addr;
  while (1)
  {
    printf("waiting for connection...\n");
    run = true;
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

    vsx_string welcome = ">>VSXu Server 0.3.0\n";

    send(
      recv_sock,
      welcome.c_str(),
      welcome.size(),
      0
    );

    memset(&recv_buf,0,BUFLEN);
    while (run)
    {
      size_recv = recv(recv_sock, &recv_buf, BUFLEN, MSG_DONTWAIT);
      if (size_recv == -1)
      {
        if (EAGAIN != errno && EWOULDBLOCK != errno)
        {
          printf("EAGAIN / EWOULDBLOCK. closing socket...\n");
          close(recv_sock);
          run = false;
        }
      }
      //printf("status: %d\n",status);

      vsx_command_s *out_command;
      int count_sent = 0;
      while (this_->cmd_out->pop(&out_command))
      {
        //printf("sending command: %s\n",out_command->str().c_str() );
        vsx_string res = out_command->str()+"\n";
        if (send(
          recv_sock,
          res.c_str(),
          res.size(),
          MSG_NOSIGNAL
        ) == -1) {
          printf("error in sending. closing socket...\n");
          close(recv_sock);
          run = false;
        } else
        {
          count_sent++;
        }
      }
      //printf("after cmd_out loop\n");
      if (!count_sent) {
        if (keepalive_timer++ == 150) {
          vsx_string res = "_\n";
          if (send(
            recv_sock,
            res.c_str(),
            res.size(),
            MSG_NOSIGNAL
          ) == -1) {
            close(recv_sock);
            run = false;
          }
          keepalive_timer = 0;
        }
      }

      // need microsecond timing calculation
      if (size_recv > 0 && size_recv < BUFLEN)
      {
        //printf("size_recv: %d\n", size_recv);
        for (ssize_t i = 0; i < size_recv; i++)
        {
          if
            (
              (recv_buf[i] == '\n' || recv_buf[i] == '\r')
              &&
              message_stack.size()
            )
          {
            //printf("got2: %s___\n",message_stack.c_str());
            if (message_stack == "dc")
            {
              memset(&recv_buf,0,BUFLEN);
              close(recv_sock);
              run = false;
              message_stack = "";
              //return 0;
            }
            else
            {
              if (message_stack != "_")
              {
                this_->cmd_in->add_raw(message_stack);
              }
            }
            message_stack = "";
          } else
          {
            message_stack.push_back(recv_buf[i]);
          }
        }
        memset(&recv_buf,0,BUFLEN);
      }
      usleep(10);
      /*if (recv_buf[0] != 0)
      {
        vsx_string recv_data(recv_buf);

        vsx_avector<vsx_string> parts;
        vsx_string deli = "\n";
        explode(recv_data, deli, parts);
        for (unsigned long i = 0; i < parts.size(); i++)
        {
          vsx_string* msg = &parts[i];
          if ((*msg)[msg->size()-1] == '\r') msg->pop_back();
          if ((*msg)[msg->size()-1] == '\n') msg->pop_back();
          if ((*msg)[msg->size()-1] == '\r') msg->pop_back();
          //printf("got2: %s___\n",msg->c_str());
          if (*msg == "dc")
          {
            close(recv_sock);
            run = false;
          }
          else
          {
            this_->cmd_in->add_raw(*msg);
          }
        }
        memset(&recv_buf,0,size_recv+0x1);
      }
      usleep(10000);*/
    }
  }
  close(listen_sock);
  return 0;
}

// ****************************************************************************
// ****************************************************************************
// VSX_COMMAND_LIST_CLIENT ****************************************************
// ****************************************************************************
// ****************************************************************************

vsx_command_list_client::vsx_command_list_client()
{
  connected = VSX_COMMAND_CLIENT_NEVER_CONNECTED;
}

void* vsx_command_list_client::client_worker(void *ptr)
{
  vsx_command_list_client* this_ = (vsx_command_list_client*)ptr;
  /*int status;*/
  struct addrinfo hints;
  struct addrinfo *servinfo;  // will point to the results
  char recv_buf[BUFLEN];
  int sock;
  ssize_t size_recv;
  int keepalive_timer = 0;
  vsx_string message_stack;
  
  memset(&hints, 0, sizeof hints); // make sure the struct is empty
  hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

  // get ready to connect
  /*status = */getaddrinfo(this_->server_address.c_str(), "11030", &hints, &servinfo);

  sock = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

  // servinfo now points to a linked list of 1 or more struct addrinfos
  if (connect(sock, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
  {
    this_->connected = VSX_COMMAND_CLIENT_DISCONNECTED;
    handle_error("connect");
  }

  int flag = 1;
  if (setsockopt(sock,            /* socket affected */
                                 IPPROTO_TCP,     /* set option at TCP level */
                                 TCP_NODELAY,     /* name of option */
                                 (char *) &flag,  /* the cast is historical
                                                         cruft */
                                 sizeof(int))    /* length of option value */
      == -1
     )
  {
    handle_error("setsockopt");
  }

  memset(&recv_buf,0,BUFLEN);
  bool run = true;

  this_->connected = VSX_COMMAND_CLIENT_CONNECTED;
  while (run)
  {
    size_recv = recv(sock, &recv_buf, BUFLEN-1, MSG_DONTWAIT);
    if (size_recv == -1)
    {
      if (EAGAIN != errno && EWOULDBLOCK != errno)
      {
        close(sock);
        run = false;
        this_->connected = VSX_COMMAND_CLIENT_DISCONNECTED;
        //printf("line: %d\n", __LINE__);
        return 0;
      }
    }
    int count_sent = 0;
    vsx_command_s *out_command;
    while (this_->cmd_out.pop(&out_command))
    {
      vsx_string res = out_command->str()+"\n";
      //printf("sending to server: %s\n", res.c_str() );
      if (send(
        sock,
        res.c_str(),
        res.size(),
        MSG_NOSIGNAL
        ) == -1)
      {
          close(sock);
          this_->cmd_out.add_front(out_command);
          this_->connected = VSX_COMMAND_CLIENT_DISCONNECTED;
          //printf("line: %d\n", __LINE__);
          return 0;
      }
      else
      {
        count_sent++;
      }
    }
    
    if (!count_sent) {
      if (keepalive_timer++ == 150) {
        vsx_string res = "_\n";
        if (send(
          sock,
          res.c_str(),
          res.size(),
          MSG_NOSIGNAL
        ) == -1) {
          close(sock);
          this_->connected = VSX_COMMAND_CLIENT_DISCONNECTED;
          run = false;
          printf("line: %d\n", __LINE__);
          return 0;
        }
        keepalive_timer = 0;
      }
    }

    // need microsecond timing calculation
    if (size_recv > 0 && size_recv < BUFLEN)
    {
      for (ssize_t i = 0; i < size_recv; i++)
      {
        if
          (
            (recv_buf[i] == '\n' || recv_buf[i] == '\r')
            &&
            message_stack.size()
          )
        {
          //printf("got2: %s___\n",message_stack.c_str());
          this_->cmd_in.add_raw(message_stack);
          message_stack = "";
        } else
        {
          if (message_stack != "_")
          {
            message_stack.push_back(recv_buf[i]);
          }
        }
      }
      memset(&recv_buf,0,BUFLEN);
    }
    usleep(10);
  }
  return 0;
}

vsx_command_list* vsx_command_list_client::get_command_list_in()
{
  return &cmd_in;
}

vsx_command_list* vsx_command_list_client::get_command_list_out()
{
  return &cmd_out;  
}

bool vsx_command_list_client::client_connect(vsx_string &server_a)
{
  server_address = server_a;
  pthread_attr_init(&worker_t_attr);
  pthread_create(&worker_t, &worker_t_attr, &client_worker, (void*)this);
  pthread_detach(worker_t);
  return true;
}

int vsx_command_list_client::get_connection_status()
{
  return connected;
}

#endif
