#include <iostream>
#include "RequestHeader.h"

int main(){
    
    string str = "GET / HTTP/1.1\r\nHost: www.baidu.com\r\n";
    RequestHeader head(str);
    map<string, string>::iterator iter = head.getHeader().begin();
    while(iter != head.getHeader().end()) {
        cout << iter->first << ": " << iter->second << endl;
        iter++;
    }
}

map<string, string>& RequestHeader::getHeader(){
    return header;
}
void RequestHeader::setupHeader(){
    header.insert(pair<string, string>("METHOD", ""));
    header.insert(pair<string, string>("URL", ""));
    header.insert(pair<string, string>("HOST", ""));
    header.insert(pair<string, string>("PORT", ""));
    //header.insert(pair<string, string>("EXPIRE", ""));
}

void RequestHeader::splitHeader(string content){
    size_t startIndex = 0;
    size_t endIndex = content.find("\r\n");
    startLine = content.substr(0, endIndex);
    startIndex = content.find("Host");
    content = content.substr(startIndex);
    endIndex = content.find("\r\n");
    hostLine = content.substr(0, endIndex);    
}

//GET http://www.man7.org/linux/man-pages/man2/recv.2.html HTTP/1.1
void RequestHeader::parseStartLine(string content){
    size_t startIndex = 0;
    size_t endIndex = content.find(" ");
    header["METHOD"] = content.substr(startIndex, endIndex);
    content = content.substr(endIndex + 1);
    endIndex = content.find(" ");
    header["URL"] = content.substr(startIndex, endIndex);
    if(header["METHOD"] == "CONNECT"){
        header["PORT"] = "443";
    }else{
        header["PORT"] = "80";
    }
    
}

void RequestHeader::parseHostLine(string content){
    size_t startIndex = content.find(" ") + 1;
    header["HOST"] = content.substr(startIndex);
}
