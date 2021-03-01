#ifndef _REQUESTHEADER_H
#define _REQUESTHEADER_H
#include <string>
#include <map>
using namespace std;
class RequestHeader{
    public:
        map<string, string> header;
        string rawData;
        string startLine;
        string hostLine;
        bool chunked;
        int contentLen;
        bool no_cache;
        bool no_store;
        string if_modified_since;
        string if_none_match;
        

        RequestHeader(string str):rawData(str), chunked(false), contentLen(0), no_cache(false), no_store(false){
            setupHeader();
            splitHeader(rawData);
            parseStartLine(startLine);
            parseHostLine(hostLine);
            parseContentLength(rawData);
            parseOtherInfo(rawData);
        }
        void setupHeader();
        void splitHeader(string content);
        void parseStartLine(string content);
        void parseHostLine(string content);
        void parseContentLength(string content);
        //parse no_cache, no_store, if_modified_since, if_none_match
        void parseOtherInfo(string content);
        map<string, string>& getHeader();
};
#endif