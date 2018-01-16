#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>

using namespace std;

#define SOCKET_FILENAME "/tmp/server.sock"

int server;

void bind_listen_socket(int &server, sockaddr_un &server_addr)
{
  // create socket
  server = socket(PF_UNIX, SOCK_STREAM, 0);
  if (!server) {
    perror("socket");
    exit(-1);
  }

  // call bind to associate the socket with our local address and
  // port
  if (bind(server, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    perror("bind");
    exit(-1);
  }

  // convert the socket to listen for incoming connections
  if (listen(server, 0) < 0) {
    perror("listen");
    exit(-1);
  }

  puts("Listening to connection...");

}

void signal_callback_handler(int signum)
{
  // close server
  close(server);
  // remove the socket file
  unlink(SOCKET_FILENAME);
  // signal handled
  exit(0);
}

int main(int argc, char **argv)
{
  struct sockaddr_un server_addr, client_addr;
  socklen_t clientlen = sizeof(client_addr);
  int client, buflen, nread;
  char *buf;

  puts("Hell World");

  // listen to SIGINT, SIGTERM, and SIGKILL
  signal(SIGINT, signal_callback_handler);
  signal(SIGTERM, signal_callback_handler);
  signal(SIGKILL, signal_callback_handler);

  // setup socket address structure
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sun_family = AF_UNIX;
  strcpy(server_addr.sun_path, SOCKET_FILENAME);

  // bind and listen on the socket file
  bind_listen_socket(server, server_addr);

  // allocate buffer
  buflen = 1024;
  buf = new char[buflen+1];

  // loop to handle all requests
  while (1) {
    unsigned int client = accept(server, (struct sockaddr *)&client_addr, &clientlen);

    // got a request, close the socket
    close(server);
    unlink(SOCKET_FILENAME);

    // read a request
    memset(buf, 0, buflen);
    nread = recv(client, buf, buflen, 0);

    printf("\nClient says: %s\n\n", buf);

    // echo back to the client
    send(client, buf, nread, 0);

    close(client);

    sleep(2);

    // re-bind and listen on the socket
    bind_listen_socket(server, server_addr);
  }

  close(server);

  unlink(SOCKET_FILENAME);

  return 0;
}
