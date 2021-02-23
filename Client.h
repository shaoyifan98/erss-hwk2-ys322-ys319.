#include <cstdlib>
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <unistd.h>
#include <string>

class Client {
  private:
    int sockfd;
    struct addrinfo * addrSrv;
    std::string hostName;
    std::string portNum;
  
  public:
    // server constructor
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

    // server destructor
    ~Client() {
      close(sockfd);
      freeaddrinfo(addrSrv);
    }

    void clientSend(std::string msg) {
      send(sockfd, msg.c_str(), msg.size(), 0);
    }

    void clientRecv() {
      char buf[1024];
      if ( recv(sockfd, buf, 1024, 0 ) == -1 ) {
        perror( "recv" );
        exit( 1 );
      }
      /* Show what the reply was*/
      printf( "%s\n", buf );
    }

    int getSockfd() {
      return sockfd;
    }

};
