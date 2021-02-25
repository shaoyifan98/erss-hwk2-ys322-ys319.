#include <string>
#include <map>
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

        ResponseHeader(string str):rawData(str), status(0), etag(""), date(""), max_age(-1), last_modified(""), expire(""), payload(""), uri(""){
          parse(rawData);
        }

        void parse(string content);
        
};



