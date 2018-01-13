#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>

using namespace std;

int server;

void bindSocket(int &server, int &reuse, sockaddr_in &server_addr)
{
  // create socket
  server = socket(PF_INET, SOCK_STREAM, 0);
  if (!server) {
    perror("socket");
    exit(-1);
  }

  // set socket to immediately reuse port when the application closes
  reuse = 1;
  if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
    perror("setsockopt");
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
  // signal handled
  exit(0);
}

int main(int argc, char **argv)
{
  struct sockaddr_in server_addr, client_addr;
  socklen_t clientlen = sizeof(client_addr);
  int client, buflen, nread, initCommResult;
  char *buf;
  char c;
  int port = 5700;
  int reuse = 1;

  puts("Hell World");

  // listen to SIGINT and SIGTERM
  signal(SIGINT, signal_callback_handler);
  signal(SIGTERM, signal_callback_handler);
  signal(SIGKILL, signal_callback_handler);

  // setup socket address structure
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  // create socket and bind it
  bindSocket(server, reuse, server_addr);

  // allocate buffer
  buflen = 1024;
  buf = new char[buflen+1];

  // loop to handle all requests
  while (1) {
    unsigned int client = accept(server, (struct sockaddr *)&client_addr, &clientlen);

    // close socket first to refuse any further connection
    close(server);
    puts("Socket closed while handling new connection");

    // read a request
    memset(buf, 0, buflen);
    nread = recv(client, buf, buflen, 0);

    printf("\nClient says: %s\n\n", buf);

    // echo back to the client
    send(client, buf, nread, 0);

    close(client);

    // re-create socket and bind it
    puts("Re-opening socket...");
    bindSocket(server, reuse, server_addr);
  }

  close(server);

  return 0;
}
