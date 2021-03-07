#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <unistd.h>
#include <string>
#include <vector>
#include "myException.h"
#include "ResponseHeader.h"


#define CHUNK_SIZE 65536

class Client {
  private:
    int sockfd;
    struct addrinfo * addrSrv;
    std::string hostName;
    std::string portNum;
    std::string ip;
  
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
        throw myException("convert failed"); 
      }
      // create socket file descriptor
      if((sockfd = socket(addrSrv->ai_family, addrSrv->ai_socktype, addrSrv->ai_protocol)) == -1) {
        throw myException("building socket failed"); 
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
      //std::cout << "client sending" << std::endl;
      int msgSize = msg.size();
        int send_size = send(sockfd, msg.c_str(), msgSize, 0);
        if (send_size < 0) {
          throw myException("Client Sending failed");
        }
    }

    // get sockfd that connects
    int getSockfd() {
      return sockfd;
    }

    // client receives http response
    std::string clientRecvResp(int clientfd) {
      std::cout << "recv response..." << std::endl;
      char buffer[CHUNK_SIZE];
      int recv_size = 0;
      std::string result = "";
      //loop to receive responseHead
      while(1) {
        memset(buffer, 0, sizeof(char));
        recv_size = recv(sockfd, buffer, CHUNK_SIZE, 0);
        int send_size = send(clientfd, buffer, recv_size, 0);
        if (send_size < 0) {
          throw myException("Server Sending failed");
        }
        if (recv_size == 0) {
          //throw myException("Client Recving failed");
          break;
        }
        std::string temp(buffer, recv_size);
        result += temp;
        if (result.find("\r\n\r\n") != std::string::npos) {
          break;
        }
      }
      int endIndex = result.find("\r\n\r\n") + 4;
      // responseHead
      std::string responseHead = result.substr(0, endIndex);
      ResponseHeader resp(responseHead);

      //for status code 1xx, 204 or 304, only response head
      // if (resp.status == 204 || resp.status == 304 || ((resp.status >= 100) && (resp.status < 200))) {
        
      //   return responseHead;
      // }

      // responseBody
      std::string responseBody = result.substr(endIndex);

      // for any other methods, need response body or chunked encoding
      // Transfer-encoding chunked
      if (resp.chunked) {
        if (responseBody.find("\r\n\r\n") == std::string::npos) {
          // keep recieving
          char buffer[CHUNK_SIZE];
          int recv_size = 0;
          //loop to receive requestBody
          while(1) {
            memset(buffer, 0, sizeof(char));
            recv_size = recv(sockfd, buffer, CHUNK_SIZE, 0);
            if (recv_size <= 0) {
              break;
            }
            int send_size = send(clientfd, buffer, recv_size, 0);
            if (send_size < 0) {
              throw myException("Server Sending failed");
            }
            std::string temp(buffer, recv_size);
            responseBody += temp;
            if (responseBody.find("\r\n\r\n") != std::string::npos) {
              break;
            }
          }
        }
      } else if(resp.contentLen != 0){ // have Content-Length
        int contentLen = resp.contentLen - responseBody.size();   
        int totalLen = 0;
        if (contentLen != 0) {
          while (totalLen < contentLen) {
            char buffer[contentLen];
            memset(buffer, 0, sizeof(char));
            int recv_size = recv(sockfd, buffer, contentLen, 0);
            if (recv_size <= 0) {
              break;
            }
            int send_size = send(clientfd, buffer, recv_size, 0);
            if (send_size < 0) {
              throw myException("Server Sending failed");
            }
            std::string temp(buffer, recv_size);
            responseBody += temp;
            totalLen += recv_size;
          }
        }
         return (responseHead + responseBody);
      }
        return (responseHead + responseBody);
    }

    std::string clientRecvResp() {
      std::cout << "recv response..." << std::endl;
      char buffer[CHUNK_SIZE];
      int recv_size = 0;
      std::string result = "";
      //loop to receive responseHead
      while(1) {
        memset(buffer, 0, sizeof(char));
        recv_size = recv(sockfd, buffer, CHUNK_SIZE, 0);
        std::string temp(buffer, recv_size);
        result += temp;
        if (result.find("\r\n\r\n") != std::string::npos) {
          break;
        }
      }
      int endIndex = result.find("\r\n\r\n") + 4;
      // responseHead
      std::string responseHead = result.substr(0, endIndex);
      ResponseHeader resp(responseHead);
      std::string responseBody = result.substr(endIndex);

      // for any other methods, need response body or chunked encoding
      // Transfer-encoding chunked
      if (resp.chunked) {
        if (responseBody.find("\r\n\r\n") == std::string::npos) {
          // keep recieving
          char buffer[CHUNK_SIZE];
          int recv_size = 0;
          //loop to receive requestBody
          while(1) {
            memset(buffer, 0, sizeof(char));
            recv_size = recv(sockfd, buffer, CHUNK_SIZE, 0);
            if (recv_size <= 0) {
              break;
            }
            std::string temp(buffer, recv_size);
            responseBody += temp;
            if (responseBody.find("\r\n\r\n") != std::string::npos) {
              break;
            }
          }
        }
      } else if(resp.contentLen != 0){ // have Content-Length
        int contentLen = resp.contentLen - responseBody.size();   
        int totalLen = 0;
        if (contentLen != 0) {
          while (totalLen < contentLen) {
            char buffer[contentLen];
            memset(buffer, 0, sizeof(char));
            int recv_size = recv(sockfd, buffer, contentLen, 0);
            if (recv_size <= 0) {
              break;
            }
            std::string temp(buffer, recv_size);
            responseBody += temp;
            totalLen += recv_size;
          }
        }
         return (responseHead + responseBody);
      }
        return (responseHead + responseBody);
    }


};
