#ifndef RESPONSEHEADER
#define RESPONSEHEADER
#include <string>
#include <map>
#include "Time.h"
using namespace std;
class ResponseHeader{
    public:
        //map<string, string> header;
        string rawData;
        size_t status;
        string etag;
        string date;
        int max_age;
        string last_modified;
        string expire;
        size_t contentLen;
        bool chunked;
        string startLine;

        //the content
        string payload;
        string uri;
        bool no_cache;
        bool no_store;
        bool must_revalidate;
        Time time;

        ResponseHeader(string str):rawData(str), status(0), etag(""), date(""), max_age(-1), last_modified(""), expire(""), payload(""), uri(""),
        no_cache(false), no_store(false), must_revalidate(false){
          parse(rawData);
        }
        ResponseHeader(){}

        void parse(string content);
        string getExpireTime();
        
};
#endif


