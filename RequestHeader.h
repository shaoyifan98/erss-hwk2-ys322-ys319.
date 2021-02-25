#include <string>
#include <map>
using namespace std;
class RequestHeader{
    public:
        map<string, string> header;
        string rawData;
        string startLine;
        string hostLine;

        RequestHeader(string str):rawData(str){
            setupHeader();
            splitHeader(rawData);
            parseStartLine(startLine);
            parseHostLine(hostLine);
        }
        void setupHeader();
        void splitHeader(string content);
        void parseStartLine(string content);
        void parseHostLine(string content);
        map<string, string>& getHeader();
};