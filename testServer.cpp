#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>
#include "Server.h"

int main() {
  Server server = Server();
  int clientfd = server.serverAccept();
  std::string str = "POST /login HTTP/1.1\r\nHost: www.example.com\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 30\r\n\r\nusername=hello&password=123456\r\n";
  RequestHeader head1(str);
  std::string req = server.serverRecvReq(clientfd);
  std::cout << req << std::endl;
  server.serverSend(clientfd, "hello");
}