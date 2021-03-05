#include <string>
#include <map>
#include "RequestHeader.h"
#include "ResponseHeader.h"
#include "Time.h"
#include <unordered_map>
#include <list>
using namespace std;
#define CAPACITY 1000   
class Cache{
    public:
        size_t count;
        Time time;
        Cache():count(0){};
        map<string, ResponseHeader> cache;
       // list<ResponseHeader> cache_list;
        void add(ResponseHeader content);
        void remove(string uri);
        bool isFresh(RequestHeader& header);
        void deleteOldRecord();
        string useCache(RequestHeader& header);
        bool canUseCache(RequestHeader& header);
};