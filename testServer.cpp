#include <cstdlib>
#include <cstdio>
#include "Server.h"
#include "Client.h"

int main() {
  Server server = Server();
  int clientfd = server.serverAccept();
  server.serverRecv(clientfd);
}