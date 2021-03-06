#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <vector>
#include "myException.h"
#include "RequestHeader.h"

#define CHUNK_SIZE 65536

class Server {
  private:
    int sockfd;
    struct addrinfo * addrSrv;
  public:
    // server constructor
    Server() {
      struct addrinfo hints;
      memset(&hints, 0, sizeof(struct addrinfo));
      hints.ai_family = AF_UNSPEC; /* Allow both IPv4 IPv6 */
      hints.ai_flags = AI_PASSIVE; /* For wildcard IP address */
      hints.ai_protocol = 0; /* Any protocol */
      hints.ai_socktype = SOCK_STREAM; /* TCP protocol */
      // convert hints into addrSrv
      if(getaddrinfo(NULL, "12345", &hints, &addrSrv)) {
        perror("convert failed"); 
        throw myException("convert failed");
      }
      // create socket file descriptor
      if((sockfd = socket(addrSrv->ai_family, addrSrv->ai_socktype, addrSrv->ai_protocol)) == -1) {
        throw myException("building socket failed"); 
      }
      // keep using the port
      int enable = 1;
      if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1) {
        throw myException("setsockopt failed");
      }
      // bind the socket file descriptor
      if (bind(sockfd, addrSrv->ai_addr, addrSrv->ai_addrlen) == -1) {
        throw myException("bind failed");
      }
      // listening
      if (listen(sockfd, 100) == -1) {
        throw myException("listen failed");
      }
    }

    // server destructor
    ~Server() {
      close(sockfd);
      freeaddrinfo(addrSrv);
    }

    // server accepts the client
    int serverAccept(std::string &ip) {
      struct sockaddr_storage addrClient;
      socklen_t addrLen = sizeof(addrClient);
      int clientfd = accept(sockfd, (struct sockaddr *) &addrClient, &addrLen);
      if (clientfd == -1) {
        perror("accept failed");
      }
      struct sockaddr_in * c_addr = (struct sockaddr_in *) &addrClient;
      char * ip_addr = inet_ntoa(c_addr->sin_addr);
      ip.assign(ip_addr);
      return clientfd;
    }

    // server sends a msg
    void serverSend(int otherSockfd, std::string msg) {
      std::cout << "server sending" << std::endl;
      int msgSize = msg.size();
      //int totalSize = 0;
      //while (totalSize < msgSize) {
        int send_size = send(otherSockfd, msg.c_str(), msgSize, 0);
        if (send_size < 0) {
          throw myException("Server Sending failed");
        }
        //totalSize += send_size;
      //}
    }

    // server receives http request head
    std::string serverRecvReq(int otherSockfd) {
      std::cout << "recv request..." << std::endl;
      char buffer[CHUNK_SIZE];
      int recv_size = 0;
      std::string result = "";
      //loop to receive requestHead
      while(1) {
        std::cout << "recv request in 98" << std::endl;
        memset(buffer, 0, sizeof(buffer));
        recv_size = recv(otherSockfd, buffer, CHUNK_SIZE, 0);
        if (recv_size < 0) {
          throw myException("Server Recving failed");
        }
        if (recv_size == 0) {
         break;
        }
        std::string temp(buffer, recv_size);
        result += temp;
        if (result.find("\r\n\r\n") != std::string::npos) {
          break;
        }
      }
      int endIndex = result.find("\r\n\r\n") + 4;
      // requestHead
      std::string requestHead = result.substr(0, endIndex);
      RequestHeader req(requestHead);

      // for "GET", only request head
      if (req.getHeader()["METHOD"] == "GET" || req.getHeader()["METHOD"] == "CONNECT") {
        return requestHead;
      }
      // requestBody
      std::string requestBody = result.substr(endIndex);
      // for any other methods, need request body
      // have Transfer-Encoding
      if (req.chunked) {
        if (requestBody.find("\r\n\r\n") == std::string::npos) {
          // keep recieving
          char buffer[CHUNK_SIZE];
          int recv_size = 0;
          //loop to receive requestBody
          while(1) {
            memset(buffer, 0, sizeof(buffer));
            recv_size = recv(otherSockfd, buffer, CHUNK_SIZE, 0);
            if (recv_size <= 0) {
              break;
            }
            std::string temp(buffer, recv_size);
            requestBody += temp;
            if (requestBody.find("\r\n\r\n") != std::string::npos) {
              break;
            }
          }
        }
      } else { // have Content-Length
        int contentLen = req.contentLen - requestBody.size();
        int totalLen = 0;
        if (contentLen != 0) {
          while(totalLen < contentLen) {
            char buffer[contentLen];
            memset(buffer, 0, sizeof(buffer));
            int recv_size = recv(otherSockfd, buffer, contentLen, 0);
            if (recv_size <= 0) {
              break;
            }
            std::string temp(buffer, recv_size);
            requestBody += temp;
            totalLen += recv_size;
          }
        }
        return (requestHead + requestBody + "\r\n");
      }
      return (requestHead + requestBody);
    }


};
