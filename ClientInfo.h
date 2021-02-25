#include <string>

class ClientInfo {
  private:
    int clientfd;
    std::string clientIP;
  public:
    ClientInfo(int fd, std::string ip): clientfd(fd), clientIP(ip) {}
    
    int getClientfd() {
      return clientfd;
    }

    std::string getClientIP() {
      return clientIP;
    }
};