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
    ClientInfo client(clientfd, ip, id++);
    std::thread t(handleReq, std::ref(client), std::ref(server));
    t.detach();
  }
}

void handleReq(ClientInfo & clientinfo, Server & server) {
  LogInfo log(std::to_string(clientinfo.getClientID()));
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
    close(clientinfo.getClientfd());
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
  // writelog : ID: Requesting "REQUEST" from SERVER
  // std::string info = "Requesting \"" + req.startLine + "\" from " + req.getHeader()["HOST"] + "\n";
  // log.writeInfo(info);
  
  // requesting from the remote server
  proxyAsClient.clientSend(request);
  std::string response = proxyAsClient.clientRecvResp();
  ResponseHeader resp(response);
  // writelog : ID: Received "RESPONSE" from SERVER
  // info = "Received \"" + resp.startLine + "\" from " + req.getHeader()["HOST"] + "\n";
  // log.writeInfo(info);

  // sending back to client
  server.serverSend(clientfd, response);
  // writelog : ID: Responding "RESPONSE"
  // info = "Responding \"" + resp.startLine + "\"" + "\n";
  // log.writeInfo(info);
}

void handleCONNECT(int clientfd, std::string request, RequestHeader &req, LogInfo &log, Server & server) {
  std::cout << "IN CONNECT" << std::endl;
  Client proxyAsClient(req.getHeader()["HOST"], req.getHeader()["PORT"]);
  // send connection response to client
  Time myTime;
  std::string connectResp = "HTTP/1.1 200 OK\r\nDate: " + myTime.getCurrentTimeStr() + " GMT\r\nContent-Length: 28\r\n\r\n";
  connectResp += "Https Connection Established\r\n";
  server.serverSend(clientfd, connectResp);
  // writelog : ID: Responding "RESPONSE"
  // std::string info = "Responding \"HTTP/1.1 200 OK\"\n";

  // set fds
  int proxyfd = proxyAsClient.getSockfd();
  fd_set readfds;
  FD_ZERO(&readfds);
  FD_SET(clientfd, &readfds);
  FD_SET(proxyfd, &readfds);
  int fdnum = max(clientfd, proxyfd) + 1;
  struct timeval waitTime;
  waitTime.tv_sec = 60;
  waitTime.tv_usec = 0; 
  while (true){
    int res = select(fdnum, &readfds, NULL, NULL, &waitTime);
    if (res <= 0) {
        break;
    }
    std::cout << "in selecting" << std::endl;
    if (FD_ISSET(clientfd, &readfds)){
      // receive request from client
      std::cout << "122" << std::endl;
      std::vector<unsigned char> buffer(65536);
      int recv_size = recv(clientfd, &buffer.data()[0], 65536, 0);
      if (recv_size <= 0) {
        return;
      }
      buffer.resize(recv_size);
      // send to server
      std::string clientReq = "";
      for(std::vector<unsigned char>::iterator iter = buffer.begin(); iter != buffer.end(); ++iter) {
        clientReq += *iter;
      }
      proxyAsClient.clientSend(clientReq);
    }
    if (FD_ISSET(proxyfd, &readfds)){
      // receive response from server
      std::cout << "123" << std::endl;
      std::vector<unsigned char> buffer(65536);
      int recv_size = recv(proxyfd, &buffer.data()[0], 65536, 0);
      if (recv_size <= 0) {
        return;
      }
      buffer.resize(recv_size);
      // send to server
      std::string serverResp = "";
      for(std::vector<unsigned char>::iterator iter = buffer.begin(); iter != buffer.end(); ++iter) {
        serverResp += *iter;
      }
      // send to client
      server.serverSend(clientfd, serverResp);
    }
    FD_ZERO(&readfds);
    FD_SET(clientfd, &readfds);
    FD_SET(proxyfd, &readfds);
  }
}
