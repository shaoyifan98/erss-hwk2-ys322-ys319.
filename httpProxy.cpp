#include "httpProxy.h"
#include "RequestHeader.h"
#include "LogInfo.h"
#include "ClientInfo.h"
#include "Client.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>

static int id = 0;

void httpProxy::init() {
  while(true) {
    std::string ip = "";
    int clientfd = server.serverAccept(ip);
    ClientInfo * client = new ClientInfo(clientfd, ip);
    std::thread t(handleReq, client); 
  }
}

void httpProxy::handleReq(ClientInfo * client) {
  ClientInfo * clientinfo = (ClientInfo *)client;
  LogInfo log(std::to_string(id));
  std::string request = server.serverRecvReq(clientinfo->getClientfd());
  RequestHeader req(request);
  std::string logmsg = "\"" + req.startLine + "\"" + " from " + clientinfo->getClientIP() + "\n";
  log.writeInfo(logmsg);
  //req.startLine
  if(req.getHeader()["METHOD"] == "GET") {
    handleGET(clientinfo->getClientfd(), request, req, log);
  } else if (req.getHeader()["METHOD"] == "POST") {
    handlePOST(clientinfo->getClientfd(), request, req, log);
  } else if (req.getHeader()["METHOD"] == "CONNECT") {
    handleCONNECT(clientinfo->getClientfd(), request, req, log);
  } else {
    // bad request
  }
}

void httpProxy::handleGET(int clientfd, std::string request, RequestHeader &req, LogInfo &log) {

}

void httpProxy::handlePOST(int clientfd, std::string request, RequestHeader &req, LogInfo &log) {
  Client proxyAsClient(req.getHeader()["HOST"], req.getHeader()["PORT"]);
  // writelog : ID: Requesting "REQUEST" from SERVER
  std::string info = "Requesting \"" + req.startLine + "\" from " + req.getHeader()["HOST"] + "\n";
  log.writeInfo(info);
  
  // requesting from the remote server
  proxyAsClient.clientSend(request);
  std::string response = proxyAsClient.clientRecvResp();
  ResponseHeader resp(response);
  // writelog : ID: Received "RESPONSE" from SERVER
  info = "Received \"" + resp.startLine + "\" from " + req.getHeader()["HOST"] + "\n";
  log.writeInfo(info);

  // sending back to client
  server.serverSend(clientfd, response);
  // writelog : ID: Responding "RESPONSE"
  info = "Responding \"" + resp.startLine + "\"" + "\n";
  log.writeInfo(info);
}

void httpProxy::handleCONNECT(int clientfd, std::string request, RequestHeader &req, LogInfo &log) {

}
