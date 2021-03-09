#include "Cache.h"
#include <iostream>

void Cache::add(ResponseHeader content){
    if(count == CAPACITY){
        deleteOldRecord();
    }
    //ensure no duplicated insertion
    if(cache.count(content.uri) == 0){
        cache.insert(pair<string, ResponseHeader>(content.uri, content));
    }
      
}


 void Cache::remove(string uri){
    cache.erase(uri);
}

bool Cache::canUseCache(RequestHeader& header, LogInfo & log){
    string uri = header.getHeader()["URI"];
    if(cache.count(uri) == 0){
        cout << "cache not found, and uri is" << uri << endl;
        return false;
    }
    return isFresh(header, log);
}

bool Cache::isFresh(RequestHeader& header, LogInfo & log){
   string uri = header.getHeader()["URI"];
   ResponseHeader res = cache[uri];
   if(res.max_age != -1){
       if(time.computeTimeDiff(res.date) > res.max_age){
            remove(uri);
            cout << "not fresh1" << endl;
          // writelog : ID: in cache, but expired at EXPIREDTIME
            tm temp = time.parseTime(res.date);
            time_t exp_time = mktime(&temp) +res.max_age;
            //exp_time += res.max_age;
            struct tm * exp_time_tm = gmtime(&exp_time);
            //const char * t = asctime(asc_time);
            std::string info = "in cache, but expired at " + *asctime(exp_time_tm);
            log.writeInfo(info);
            return false; 
       }
   }
   if(res.expire != ""){
       if(time.computeTimeDiff(res.expire) > 0){
          remove(uri);
          cout << "not fresh2" << endl;
           // writelog : ID: in cache, but expired at EXPIREDTIME
          std::string info = "in cache, but expired at " + res.expire;
          log.writeInfo(info);
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

