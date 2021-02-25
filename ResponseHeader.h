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
        size_t max_age;
        string last_modified;
        string expire;

        ResponseHeader(string str):rawData(str){
          parse(rawData);
        }

        void parse(string content);
        
};


