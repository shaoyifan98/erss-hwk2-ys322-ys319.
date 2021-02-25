#include "Server.h"

class httpProxy {
  private:
    Server server;
  public:
    httpProxy() {
      server = Server();
    }
    void init();
    void handleReq(ClientInfo * client);
    void handleGET(int clientfd, std::string request, RequestHeader &req, LogInfo &log);
    void handlePOST(int clientfd, std::string request, RequestHeader &req, LogInfo &log);
    void handleCONNECT(int clientfd, std::string request, RequestHeader &req, LogInfo &log);
};