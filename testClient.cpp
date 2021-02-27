#include <cstdlib>
#include <cstdio>
#include "Client.h"

int main() {
  Client client("vcm-18218.vm.duke.edu", "12345");
  //Client client("www.google.com", "80");
  //client.clientSend("POST /login HTTP/1.1\r\nHost: www.example.com\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 30\r\n\r\nusername=hello&password=123456\r\n");
  client.clientSend("GET /index.html HTTP/1.1\r\nHost: www.google.com\r\n\r\n");
  std::string resp = client.clientRecvResp();
  std::cout << resp << std::endl;
  return EXIT_SUCCESS;
}