#include "Cache.h"


void Cache::add(string uri, ResponseHeader content){
    if(count == CAPACITY){
        deleteOldRecord();
    }
    content.uri = uri;
    cache_list.push_back(content);
    cache.insert(pair<string, list<ResponseHeader>::iterator>(uri, --cache_list.end()));
    ++count;
    
}


 void Cache::remove(string uri){
    cache_list.erase(cache[uri]);
    cache.erase(uri);
}

bool Cache::isFresh(RequestHeader& header){
   string uri = header.getHeader()["URI"];
   ResponseHeader& res = *cache[uri];
   if(res.max_age != -1){
       if(time.computeTimeDiff(res.date) > res.max_age){
           remove(uri);
           return false; 
       }
   }
   if(res.expire != ""){
       if(time.computeTimeDiff(res.expire) > 0){
           remove(uri);
           return false;
       }
   }
   return true;
}

 void Cache::deleteOldRecord(){
    list<ResponseHeader>::iterator it = cache_list.begin();
    for(int i = 0; i < CAPACITY; ++i){
        cache.erase(it->uri);
        cache_list.erase(it++);
    }
}

string Cache::useCache(RequestHeader& header){
    string uri = header.getHeader()["URI"];
    list<ResponseHeader>::iterator it = cache[uri];
    ResponseHeader temp = *it;
    cache_list.erase(it);
    cache_list.push_back(temp);
    return temp.payload;
}

