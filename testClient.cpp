#include <cstdlib>
#include <cstdio>
#include "Server.h"
#include "Client.h"

int main() {
  Client client("www.google.com", "80");
  client.clientSend("GET /index.html HTTP/1.1\r\nHost: www.google.com\r\n\r\n");
  client.clientRecv();
}