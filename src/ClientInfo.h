#include <string>

class ClientInfo {
  private:
    int clientfd;
    std::string clientIP;
    int clientID;

  public:
    ClientInfo(int fd, std::string ip, int id): clientfd(fd), clientIP(ip), clientID(id) {}
    
    int getClientfd() {
      return clientfd;
    }

    std::string getClientIP() {
      return clientIP;
    }

    int getClientID() {
      return clientID;
    }
};