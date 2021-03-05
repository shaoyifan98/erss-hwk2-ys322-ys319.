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
#include <signal.h>
#include "Cache.h"
#include "ResponseHeader.h"


void handleReq(ClientInfo & clientinfo, Server & server);
void handleGET(int clientfd, std::string request, RequestHeader &req, LogInfo &log, Server & server);
void handlePOST(int clientfd, std::string request, RequestHeader &req, LogInfo &log, Server & server);
void handleCONNECT(int clientfd, std::string request, RequestHeader &req, LogInfo &log, Server & server);
static Cache cache;
void httpProxy::init() {
  int id = 0;
  signal(SIGPIPE, SIG_IGN);
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
 // while(true){
  std::string request;
  try{
    LogInfo log(std::to_string(clientinfo.getClientID()));
    std::cout << clientinfo.getClientID() << std::endl;
    try{
       request = server.serverRecvReq(clientinfo.getClientfd());
    }
    catch(const std::exception& e){
      std::cerr << e.what() << '\n';
      return;
    }
    if (request.find("HTTP") == string::npos) {
      //std::cout << "in38" << std::endl;
       return ;
    }
    //std::cout << "head is " << request << std::endl;
    RequestHeader req(request);
    /*
    // write loginfo: ID: "REQUEST" from IPFROM @ TIME
    std::string logmsg = "\"" + req.startLine + "\"" + " from " + clientinfo.getClientIP();
    Time myTime;
    logmsg += " @ " + myTime.getCurrentTimeStr();
    log.writeInfo(logmsg);
    */
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
      return;
    }
  }catch(const std::exception& e){
      std::cerr << e.what() << '\n';
      send(clientinfo.getClientfd(), "HTTP/1.1 400 Bad Request\r\n\r\n", 100, 0);
    }
  }
  
bool validate(RequestHeader &req, string response,  ResponseHeader& resHeader){
  Client proxyAsClient(req.getHeader()["HOST"], req.getHeader()["PORT"]);
  string newRequest = req.startLine + "\r\n" + req.hostLine + "\r\n";
  if(resHeader.etag != ""){
    newRequest.append("If-None-Match: \"" + resHeader.etag + "\"\r\n");
  }
  if(resHeader.last_modified != ""){
     newRequest.append("If-Modified-Since: " + resHeader.last_modified +"\r\n");
  }
  newRequest.append("\r\n");
  proxyAsClient.clientSend(newRequest);
  string newResponse = proxyAsClient.clientRecvResp();
  if(newResponse.find("304 Not Modified") != string::npos){
    return true;
  }
  return false;
}

void handleGET(int clientfd, std::string request, RequestHeader &req, LogInfo &log, Server &server) {
  std::string response;
    cout << "get" << endl;
    cout << "uri is" << req.getHeader()["URI"] << endl;
  if(!req.no_store && cache.canUseCache(req)){
    cout << "enter 103" << endl;
    response = cache.useCache(req);
    //ResponseHeader resHeader();
    //if(req.no_cache || req.no_store ){
      if(req.no_cache ){
          cout << "enter 108" << endl;
      if(validate(req, response, cache.cache[req.getHeader()["URI"]])){
        //can use content in the cache
        send(clientfd, response.c_str(), response.length(), 0);
        cout << "cache is used" << endl;
        //cout << response << endl;
        return;  
      }else{
        Client proxyAsClient(req.getHeader()["HOST"], req.getHeader()["PORT"]);
        proxyAsClient.clientSend(request);
        response = proxyAsClient.clientRecvResp(clientfd);
      }
    }else{
      //use cache
        send(clientfd, response.c_str(), response.length(), 0);
        cout << "cache is used" << endl;
        return;  
    }
  }else{
    Client proxyAsClient(req.getHeader()["HOST"], req.getHeader()["PORT"]);
    proxyAsClient.clientSend(request);
    response = proxyAsClient.clientRecvResp(clientfd);
  }
   
  //std::cout << response << std::endl;
  
  ResponseHeader resp(response);
  if(!resp.no_store && resp.max_age != 0){
    resp.uri = req.getHeader()["URI"];
    cache.add(resp);
    cout << "add in to cache, and uri is" << resp.uri << endl;

    //cout << response << endl;
  }
}

void handlePOST(int clientfd, std::string request, RequestHeader &req, LogInfo &log, Server & server) {
  Client proxyAsClient(req.getHeader()["HOST"], req.getHeader()["PORT"]);
  proxyAsClient.clientSend(request);
  std::string response = proxyAsClient.clientRecvResp(clientfd);
  ResponseHeader resp(response);
  server.serverSend(clientfd, response);
}

void handleCONNECT(int clientfd, std::string request, RequestHeader &req, LogInfo &log, Server & server) {
  std::cout << "IN CONNECT" << std::endl;
  Client proxyAsClient(req.getHeader()["HOST"], req.getHeader()["PORT"]);
  cout << "HOST IS :" << req.getHeader()["HOST"] << "Port is:" <<req.getHeader()["PORT"] << endl;
  send(clientfd, "HTTP/1.1 200 OK\r\n\r\n", 64, 0);
  // set fds
  int proxyfd = proxyAsClient.getSockfd();
  struct timeval waitTime;
  waitTime.tv_sec = 15;
  waitTime.tv_usec = 0; 
  fd_set readfds;
  //need to be replaced
  while (true) {
    FD_ZERO(&readfds);
    FD_SET(clientfd, &readfds);
    FD_SET(proxyfd, &readfds);
    int fdnum = max(clientfd, proxyfd) + 1;
    select(fdnum, &readfds, NULL, NULL, &waitTime);
    int fdArr[2] = {clientfd, proxyfd};
    int len;
    for (int i = 0; i < 2; i++) {
      char data[65536];
      if (FD_ISSET(fdArr[i], &readfds)) {
        len = recv(fdArr[i], data, sizeof(data), 0);
        if (len <= 0) {
          return;
        }
        else {
          if (send(fdArr[1 - i], data, len, 0) <= 0) {
            return;
          }
        }
      }
    }
  }
}
