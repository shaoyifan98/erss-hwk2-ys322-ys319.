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

#define CHUNK_SIZE 1024

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

    // get sockfd that connects
    int getSockfd() {
      return sockfd;
    }

    // client receives http response
    std::string clientRecvResp() {
      std::vector<unsigned char> buffer(CHUNK_SIZE);
      int recv_size = 0;
      std::string result = "";
      //loop to receive responseHead
      while(1) {
        recv_size = recv(sockfd, &buffer[0], CHUNK_SIZE, 0);
        if (recv_size < 0) {
          throw myException("Client Recving failed");
        }
        buffer.resize(recv_size);
        // write into string
        for(std::vector<unsigned char>::iterator iter = buffer.begin(); iter != buffer.end(); ++iter) {
          result += *iter;
        }
        if (result.find("\r\n\r\n") != std::string::npos) {
          break;
        }
      }
      int endIndex = result.find("\r\n\r\n") + 4;
      // responseHead
      std::string responseHead = result.substr(0, endIndex);
      // responseBody
      std::string responseBody = result.substr(endIndex);
      ResponseHeader resp(responseHead);

      // for status code 1xx, 204 or 304, only response head
      if (resp.status == 204 || resp.status == 304 || ((resp.status >= 100) && (resp.status < 200))) {
        return responseHead;
      }

      // for any other methods, need response body or chunked encoding
      // Transfer-encoding chunked
      if (resp.chunked) {
        if (responseBody.find("0\r\n\r\n") == std::string::npos) {
          // keep recieving
          std::vector<unsigned char> buffer(CHUNK_SIZE);
          int recv_size = 0;
          //loop to receive requestBody
          while(1) {
            recv_size = recv(sockfd, &buffer[0], CHUNK_SIZE, 0);
            if (recv_size < 0) {
              throw myException("Server Recving failed");
            }
            buffer.resize(recv_size);
            // write into string
            for(std::vector<unsigned char>::iterator iter = buffer.begin(); iter != buffer.end(); ++iter) {
              responseBody += *iter;
            }
            if (responseBody.find("0\r\n\r\n") != std::string::npos) {
              break;
            }
          }
        }
      } else { // have Content-Length
        int contentLen = resp.contentLen - responseBody.size() + 2;   //  add "\r\n"
        if (contentLen != 0) {
          std::vector<unsigned char> buffer(contentLen);
          int recv_size = recv(sockfd, &buffer[0], contentLen, 0);
          if (recv_size < 0) {
            throw myException("Client Recving failed");
          }
          for(std::vector<unsigned char>::iterator iter = buffer.begin(); iter != buffer.end(); ++iter) {
            responseBody += *iter;
          }
        }
      }
      return (responseHead + responseBody);
    }

};
