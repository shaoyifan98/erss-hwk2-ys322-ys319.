#include <iostream>
#include "RequestHeader.h"
#include "myException.h"
// int main(){
    
//     string str = "GET / HTTP/1.1\r\nHost: www.baidu.com\r\nCache-Control: no-cache\r\nIf-Modified-Since: Wed, 21 Oct 2018 07:28:00 GMT\r\nIf-None-Match: \"33a64df\"";
//     RequestHeader head(str);
//     //map<string, string>::iterator iter = head.getHeader().begin();
//     // while(iter != head.getHeader().end()) {
//     //     cout << iter->first << ": " << iter->second << endl;
//     //     iter++;
//     // }
//     // string str = "123sdfsdfsdf123";
//     // int i = atoi(str.c_str());
//     // cout << i << endl;
//     cout << head.if_modified_since << endl;
//     cout << head.if_none_match << endl;
//     if(head.no_cache){
//         cout << "no cache" << endl;
//     }
// }

map<string, string>& RequestHeader::getHeader(){
    return header;
}
void RequestHeader::setupHeader(){
    header.insert(pair<string, string>("METHOD", ""));
    header.insert(pair<string, string>("URI", ""));
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
    header["URI"] = content.substr(startIndex, endIndex);
    if(header["METHOD"] == "CONNECT"){
        header["PORT"] = "443";
    }else{
        header["PORT"] = "80";
    }
}

void RequestHeader::parseHostLine(string content){
    size_t startIndex = content.find(" ") + 1;
    string host = content.substr(startIndex);
    size_t endIndex = host.find(":");
    if(endIndex == string::npos){
      endIndex = content.length();
    }
    header["HOST"] = content.substr(startIndex, endIndex);
}

void RequestHeader::parseContentLength(string content) {
    size_t startIndex = content.find("Content-Length:");
    // have Content-Length
    if (startIndex != std::string::npos) {
        startIndex += 16;
        string contentLength = content.substr(startIndex);
        size_t endIndex = contentLength.find_first_of("\r\n");
        contentLength = contentLength.substr(0, endIndex);
        contentLen = atoi(contentLength.c_str());
        chunked = false;
    } else if (content.find("Transfer-Encoding: chunked") != std::string::npos) {
        chunked = true;
    }
}

void RequestHeader::parseOtherInfo(string content){
    if(content.find("no-cache") != string::npos){
        no_cache = true;
    }
    if(content.find("no-store") != string::npos){
        no_store = true;
    }
    size_t index;
    if((index = content.find("If-Modified-Since:")) != string::npos){
        string temp = content.substr(index);
        size_t start = temp.find(" ") + 1;
        size_t end = temp.find(" GMT");
        if_modified_since = temp.substr(start, end - start + 1);
    }
    if((index = content.find("If-None-Match: ")) != string::npos){
       string temp = content.substr(index);
        size_t start = temp.find("\"") + 1;
        temp = temp.substr(start);
        size_t end = temp.find("\"");
        if_none_match = temp.substr(0, end);
    }
}
