#include "httpProxy.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>
#include "Time.h"
#include "Client.h"
#include "LogInfo.h"
#include "ClientInfo.h"

void handleReq(ClientInfo & clientinfo, Server & server);
void handleGET(int clientfd, std::string request, RequestHeader &req, LogInfo &log, Server & server);
void handlePOST(int clientfd, std::string request, RequestHeader &req, LogInfo &log, Server & server);
void handleCONNECT(int clientfd, std::string request, RequestHeader &req, LogInfo &log, Server & server);

void httpProxy::init() {
  int id = 0;
  while(true) {
    std::string ip = "";
    int clientfd = server.serverAccept(ip);
    std::mutex writeLock;
    writeLock.lock();
    ClientInfo client(clientfd, ip, id++);
    writeLock.unlock();
    std::thread t(handleReq, std::ref(client), std::ref(server));
    t.detach();
  }
}

void handleReq(ClientInfo & clientinfo, Server & server) {
  try
  {
    LogInfo log(std::to_string(clientinfo.getClientID()));
    std::cout << clientinfo.getClientID() << std::endl;
    std::string request = server.serverRecvReq(clientinfo.getClientfd());
    std::cout << request << std::endl;
    RequestHeader req(request);
    /*
    // write loginfo: ID: "REQUEST" from IPFROM @ TIME
    std::string logmsg = "\"" + req.startLine + "\"" + " from " + clientinfo.getClientIP();
    Time myTime;
    logmsg += " @ " + myTime.getCurrentTimeStr();
    log.writeInfo(logmsg);
    */
    
    //req.startLine
    if(req.getHeader()["METHOD"] == "GET") {
      std::cout << "get" << std::endl;
      handleGET(clientinfo.getClientfd(), request, req, log, server);
    } else if (req.getHeader()["METHOD"] == "POST") {
      std::cout << "post" << std::endl;
      handlePOST(clientinfo.getClientfd(), request, req, log, server);
    } else if (req.getHeader()["METHOD"] == "CONNECT") {
      std::cout << "connect" << std::endl;
      handleCONNECT(clientinfo.getClientfd(), request, req, log, server);
    } else {
      // bad request, close socket
      return;
    }
    return;
  }
  catch(const std::exception& e)
  {
    std::cerr << e.what() << '\n';
  }
}

void handleGET(int clientfd, std::string request, RequestHeader &req, LogInfo &log, Server &server) {
  Client proxyAsClient(req.getHeader()["HOST"], req.getHeader()["PORT"]);
  
  // requesting from the remote server
  proxyAsClient.clientSend(request);
  std::string response = proxyAsClient.clientRecvResp();
  std::cout << response << std::endl;
  ResponseHeader resp(response);

  // sending back to client
  server.serverSend(clientfd, response);
}

void handlePOST(int clientfd, std::string request, RequestHeader &req, LogInfo &log, Server & server) {
  Client proxyAsClient(req.getHeader()["HOST"], req.getHeader()["PORT"]);
  proxyAsClient.clientSend(request);
  std::string response = proxyAsClient.clientRecvResp();
  ResponseHeader resp(response);
  server.serverSend(clientfd, response);
}

void handleCONNECT(int clientfd, std::string request, RequestHeader &req, LogInfo &log, Server & server) {
  std::cout << "IN CONNECT" << std::endl;
  Client proxyAsClient(req.getHeader()["HOST"], req.getHeader()["PORT"]);
  // send connection response to client
  // Time myTime;
  // std::string connectResp = "HTTP/1.1 200 OK\r\nDate: " + myTime.getCurrentTimeStr() + " GMT\r\nContent-Length: 28\r\n\r\n";
  std::string connectResp = "HTTP/1.1 200 OK\r\n\r\n";
  send(clientfd, connectResp.c_str(), connectResp.length(), 0);
  //server.serverSend(clientfd, connectResp);
  // writelog : ID: Responding "RESPONSE"
  // std::string info = "Responding \"HTTP/1.1 200 OK\"\n";

  // set fds
  int proxyfd = proxyAsClient.getSockfd();
  fd_set readfds;
  // struct timeval waitTime;
  // waitTime.tv_sec = 60;
  // waitTime.tv_usec = 0; 
  while (true){
    FD_ZERO(&readfds);
    FD_SET(clientfd, &readfds);
    FD_SET(proxyfd, &readfds);
    int fdnum = max(clientfd, proxyfd) + 1;
    int res = select(fdnum, &readfds, NULL, NULL, NULL);
    if (res <= 0) {
        break;
    }
    if (FD_ISSET(clientfd, &readfds)){
      // receive request from client
      std::cout << "CONNECT - from client" << std::endl;
      char buffer[65536];
      memset(buffer, 0, sizeof(buffer));
      int recv_size = recv(clientfd, buffer, 65536, 0);
      if (recv_size <= 0) {
        return;
      }
      // send to server
      if (send(proxyfd, buffer, recv_size, MSG_NOSIGNAL) <= 0) {
        return;
      }
    }
    if (FD_ISSET(proxyfd, &readfds)){
      // receive response from server
      std::cout << "CONNECT - from server" << std::endl;
      char buffer[65536];
      memset(buffer, 0, sizeof(buffer));
      int recv_size = recv(proxyfd, buffer, 65536, 0);
      if (recv_size <= 0) {
        return;
      }
      // send to client
      if (send(clientfd, buffer, recv_size, MSG_NOSIGNAL) <= 0) {
        return;
      }
    }
  }
}
