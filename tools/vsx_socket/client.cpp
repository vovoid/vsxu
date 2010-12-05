#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

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

int sock;

int main(int argc, char* argv[])
{
  /*app_argc = argc;
  for (int i = 1; i < argc; i++) {
    vsx_string arg1 = argv[i];
    if (arg1 == "--help")
    {
      printf(
             "Usage:\n"
            );
      exit(0);
    } else
    if (arg1 == "-s") {
      if (i+1 < argc)
      {
        i++;
        vsx_string arg2 = argv[i];
        vsx_avector<vsx_string> parts;
        vsx_string deli = ",";
        explode(arg2, deli, parts);
        x_res = s2i(parts[0]);
        y_res = s2i(parts[1]);
      }
    }
  }
*/
  int status;
  struct addrinfo hints;
  struct addrinfo *servinfo;  // will point to the results

  memset(&hints, 0, sizeof hints); // make sure the struct is empty
  hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

  // get ready to connect
  status = getaddrinfo("127.0.0.1", "4242", &hints, &servinfo);

  sock = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

  // servinfo now points to a linked list of 1 or more struct addrinfos
  connect(sock, servinfo->ai_addr, servinfo->ai_addrlen);

  char *msg = "Beej was here!";
  int len, bytes_sent;
  len = strlen(msg);
  bytes_sent = send(sock, msg, len, 0);

  return 0;
}
