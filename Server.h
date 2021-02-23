#include <cstdlib>
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>

class Server {
  private:
    int sockfd;
    struct addrinfo * addrSrv;
    char * ip_addr;
  
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
      if(getaddrinfo(NULL, "80", &hints, &addrSrv)) {
        perror("convert failed"); 
      }
      // create socket file descriptor
      if((sockfd = socket(addrSrv->ai_family, addrSrv->ai_socktype, addrSrv->ai_protocol)) == -1) {
        perror("building socket failed"); 
      }
      // keep using the port
      int enable = 1;
      if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1) {
        perror("setsockopt failed");
      }
      // bind the socket file descriptor
      if (bind(sockfd, addrSrv->ai_addr, addrSrv->ai_addrlen) == -1) {
        perror("bind failed");
      }
      // listening
      if (listen(sockfd, 100) == -1) {
        perror("listen failed");
      }
    }

    // server destructor
    ~Server() {
      close(sockfd);
      freeaddrinfo(addrSrv);
    }

    // server accept the client
    int serverAccept() {
      struct sockaddr_storage addrClient;
      socklen_t addrLen = sizeof(addrClient);
      int clientfd = accept(sockfd, (struct sockaddr *) &addrClient, &addrLen);
      if (clientfd == -1) {
        perror("accept failed");
      }
      struct sockaddr_in * c_addr = (struct sockaddr_in *) &addrClient;
      ip_addr = inet_ntoa(c_addr->sin_addr);
      return clientfd;
    }

    void serverSend(int otherSockfd, std::string msg) {
      send(otherSockfd, msg.c_str(), msg.size(), 0);
    }

    void serverRecv(int otherSockfd) {
      char buf[1024];
      if ( recv(otherSockfd, buf, 1024, 0 ) == -1 ) {
        perror( "recv" );
        exit( 1 );
      }
      /* Show what the reply was*/
      printf( "%s\n", buf );
    }
  
    

};
