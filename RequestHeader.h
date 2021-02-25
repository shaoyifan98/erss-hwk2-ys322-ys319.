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

        RequestHeader(string str):rawData(str){
            setupHeader();
            splitHeader(rawData);
            parseStartLine(startLine);
            parseHostLine(hostLine);
            parseContentLength(rawData);
        }
        void setupHeader();
        void splitHeader(string content);
        void parseStartLine(string content);
        void parseHostLine(string content);
        void parseContentLength(string content);
        map<string, string>& getHeader();
};