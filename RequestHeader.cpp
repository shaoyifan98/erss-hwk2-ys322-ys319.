#include <iostream>
#include "RequestHeader.h"
#include "myException.h"


map<string, string>& RequestHeader::getHeader(){
    return header;
}
void RequestHeader::setupHeader(){
    header.insert(pair<string, string>("METHOD", ""));
    header.insert(pair<string, string>("URI", ""));
    header.insert(pair<string, string>("HOST", ""));
    header.insert(pair<string, string>("PORT", ""));
}

void RequestHeader::splitHeader(string content){
    size_t startIndex = 0;
    size_t endIndex = content.find("\r\n");
    if(endIndex == string::npos){
        //cout << "content is" <<endl;
    //cout << content << endl;
        throw myException("bad head1");
    }
    
    startLine = content.substr(0, endIndex);
    startIndex = content.find("Host");
    if(startIndex == string::npos){
        throw myException("bad head2");
    }
    content = content.substr(startIndex);
    endIndex = content.find("\r\n");
    if(endIndex == string::npos){
        throw myException("bad head3");
    }
    hostLine = content.substr(0, endIndex);    
}

//GET http://www.man7.org/linux/man-pages/man2/recv.2.html HTTP/1.1
void RequestHeader::parseStartLine(string content){
    size_t startIndex = 0;
    size_t endIndex = content.find(" ");
    if(endIndex == string::npos){
        throw myException("bad head4");
    }
    header["METHOD"] = content.substr(startIndex, endIndex);
    content = content.substr(endIndex + 1);
    endIndex = content.find(" ");
    if(endIndex == string::npos){
         throw myException("bad head5");
    }
    header["URI"] = content.substr(startIndex, endIndex);
    if(header["METHOD"] == "CONNECT"){
        header["PORT"] = "443";
    }else{
        header["PORT"] = "80";
    }
}

void RequestHeader::parseHostLine(string content){
    size_t startIndex = content.find(" ") + 1;
    if(startIndex == string::npos){
        throw myException("bad head6");
    }
    string host = content.substr(startIndex);
    size_t endIndex = host.find(":");
    if(endIndex == string::npos){
      endIndex = content.length();
    }
    header["HOST"] = content.substr(startIndex, endIndex);
}

void RequestHeader::parseContentLength(string content) {
    size_t startIndex = content.find("Content-Length:");
    if (startIndex != std::string::npos) {
        startIndex += 16;
        string contentLength = content.substr(startIndex);
        size_t endIndex = contentLength.find_first_of("\r\n");
        contentLength = contentLength.substr(0, endIndex);
        contentLen = atoi(contentLength.c_str());
        chunked = false;
    } else if (content.find("Transfer-Encoding: chunked") != std::string::npos) {
        chunked = true;
        contentLen = 0;
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

    if((index = content.find("ETag: ")) != string::npos){
        string temp = content.substr(index);
        size_t start = temp.find("\"") + 1;
        temp = temp.substr(start);
        size_t end = temp.find("\"");
        etag = temp.substr(0, end);
    }

}
