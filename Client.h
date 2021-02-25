#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <unistd.h>
#include <string>
#include "myException.h"

#define CHUNK_SIZE 1024

class Client {
  private:
    int sockfd;
    struct addrinfo * addrSrv;
    std::string hostName;
    std::string portNum;
  
  public:
    // client constructor
    Client(std::string host, std::string port): hostName(host), portNum(port) {
      struct addrinfo hints;
      memset(&hints, 0, sizeof(struct addrinfo));
      hints.ai_family = AF_UNSPEC; /* Allow both IPv4 IPv6 */
      hints.ai_flags = AI_PASSIVE; /* For wildcard IP address */
      hints.ai_protocol = 0; /* Any protocol */
      hints.ai_socktype = SOCK_STREAM; /* TCP protocol */
      // convert hints into addrSrv
      if(getaddrinfo(hostName.c_str(), portNum.c_str(), &hints, &addrSrv)) {
        perror("convert failed"); 
      }
      // create socket file descriptor
      if((sockfd = socket(addrSrv->ai_family, addrSrv->ai_socktype, addrSrv->ai_protocol)) == -1) {
        perror("building socket failed"); 
      }
      connect(sockfd, addrSrv->ai_addr, addrSrv->ai_addrlen);
    }

    // client destructor
    ~Client() {
      close(sockfd);
      freeaddrinfo(addrSrv);
    }

    // client sends a msg
    void clientSend(std::string msg) {
      std::cout << "client sending" << std::endl;
      int msgSize = msg.size();
      char * ptr = (char*)msg.c_str();
      while (msgSize > 0) {
        int i = send(sockfd, ptr, msgSize, 0);
        if (i < 1) {
          throw myException("Client Sending failed");
        }
        ptr += i;
        msgSize -= i;
      }
    }

    // client receives http response
    std::string clientRecv() {
      std::cout << "client recving" << std::endl;
      int recv_size = 0;
      char chunk[CHUNK_SIZE];
      memset(&chunk ,0 , CHUNK_SIZE);
      std::string result = "";
      recv_size = recv(sockfd, chunk, CHUNK_SIZE, 0);
      if (recv_size < 0) {
        throw myException("Client Recving failed");
      }
      printf("%s\n", chunk);
      result += chunk;
      return result;
    }

    // get sockfd that connects
    int getSockfd() {
      return sockfd;
    }

};
