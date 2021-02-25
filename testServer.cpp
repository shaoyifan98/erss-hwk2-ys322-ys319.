#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>
#include "Server.h"

int main() {
  Server server = Server();
  std::string ip = "";
  int clientfd = server.serverAccept(ip);
  std::cout << "from IP addr: " << ip << std::endl;
  std::string str = "POST /login HTTP/1.1\r\nHost: www.example.com\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 30\r\n\r\nusername=hello&password=123456\r\n";
  RequestHeader head1(str);
  std::string req = server.serverRecvReq(clientfd);
  std::cout << req << std::endl;
  //str = "HTTP/1.1 200 OK\r\nDate: Mon, 27 Jul 2009 12:28:53 GMT\r\nServer: Apache/2.2.14 (Win32)\r\nLast-Modified: Wed, 22 Jul 2009 19:15:56 GMT\r\nContent-Length: 10\r\nContent-Type: text/html\r\nConnection: Closed\r\n\r\n";
  //str += "helloworld\r\n";
  str = "HTTP/1.1 200 OK\r\nDate: Mon, 27 Jul 2009 12:28:53 GMT\r\nServer: Apache/2.2.14 (Win32)\r\nLast-Modified: Wed, 22 Jul 2009 19:15:56 GMT\r\nTransfer-Encoding: chunked\r\nContent-Type: text/html\r\nConnection: Closed\r\n\r\n";
  str += "1\r\na\r\n4\r\nabcd\r\n0\r\n\r\n";
  server.serverSend(clientfd, str);
}