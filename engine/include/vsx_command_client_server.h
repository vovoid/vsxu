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
#include <vsx_string.h>
#include <vsx_command.h>

#define VSX_COMMAND_CLIENT_NEVER_CONNECTED 0
#define VSX_COMMAND_CLIENT_CONNECTED 1
#define VSX_COMMAND_CLIENT_DISCONNECTED 2

class vsx_command_list_server
{
  pthread_t         worker_t;
  pthread_attr_t    worker_t_attr;
  vsx_command_list* cmd_in;
  vsx_command_list* cmd_out;

  // internal worker method
  static void* server_worker(void *ptr);

public:
  vsx_command_list_server();
  // set the command lists on which the server class operates
  void set_command_lists(vsx_command_list* new_in, vsx_command_list* new_out);
  // start the server (after setting command_lists)
  bool start();
};


class vsx_command_list_client
{
  pthread_t         worker_t;
  pthread_attr_t    worker_t_attr;
  vsx_command_list cmd_in;
  vsx_command_list cmd_out;

  // internal server address
  vsx_string server_address;
  
  // internal worker method
  static void* client_worker(void *ptr);
  int connected;
public:
  vsx_command_list_client();
  // set the command lists on which the server class operates
  vsx_command_list* get_command_list_in();
  vsx_command_list* get_command_list_out();

  // start the server (after setting command_lists)
  bool client_connect(vsx_string &server_a);

  // get connection status
  int get_connection_status();
};