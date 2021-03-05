#include "Cache.h"


void Cache::add(ResponseHeader content){
    if(count == CAPACITY){
        deleteOldRecord();
    }
    cache.insert(pair<string, ResponseHeader>(content.uri, content));   
}


 void Cache::remove(string uri){
    cache.erase(uri);
}

bool Cache::canUseCache(RequestHeader& header){
    string uri = header.getHeader()["URI"];
    if(cache.count(uri) == 0){
        return false;
    }
    return isFresh(header);
}

bool Cache::isFresh(RequestHeader& header){
   string uri = header.getHeader()["URI"];
   ResponseHeader res = cache[uri];
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
    cache.clear();
}

string Cache::useCache(RequestHeader& header){
    string uri = header.getHeader()["URI"];
    return cache[uri].payload;
}

