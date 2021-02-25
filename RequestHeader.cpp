#include <iostream>
#include "RequestHeader.h"

/*
int main(){
    
    string str = "GET / HTTP/1.1\r\nHost: www.baidu.com\r\n";
    RequestHeader head(str);
    map<string, string>::iterator iter = head.getHeader().begin();
    while(iter != head.getHeader().end()) {
        cout << iter->first << ": " << iter->second << endl;
        iter++;
    }
    str = "POST /login HTTP/1.1\r\nHost: www.example.com\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 30\r\n\r\nusername=hello&password=123456\r\n";
    RequestHeader head1(str);
    iter = head1.getHeader().begin();
    while(iter != head1.getHeader().end()) {
        cout << iter->first << ": " << iter->second << endl;
        iter++;
    }
    cout << head1.chunked << endl;
    cout << head1.contentLen << endl;
}
*/

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

void RequestHeader::parseContentLength(string content) {
    size_t startIndex = content.find("Content-Length:");
    // have Content-Length
    if (startIndex != std::string::npos) {
        chunked = false;
        startIndex += 16;
        string contentLength = content.substr(startIndex);
        size_t endIndex = contentLength.find_first_of("\r\n");
        contentLength = contentLength.substr(0, endIndex);
        contentLen = atoi(contentLength.c_str());
    } else { // have Transfer-Encoding
        chunked = true;
        contentLen = 0;
    }
}
