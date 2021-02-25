#include <iostream>
#include "ResponseHeader.h"

int main(){
    string str = "http//1.1 200\r\nLast-Modified: Wed, 21 Oct 2015 07:28:00 GMT\r\nDate: Wed, 21 Oct 2018 07:28:00 GMT\r\nCache-Control: max-age=31536000\r\nExpires: Wed, 21 Oct 2018 07:28:00 GMT\r\nETag: \"33a64df551425fcc55e4d42a148795d9f25f89d4\"\r\n";
    ResponseHeader head(str);
    cout << head.date << endl;
    cout << head.max_age << endl;
    cout << head.status << endl;
    cout << head.last_modified << endl;

}

 void ResponseHeader::parse(string content){
    //get response status
   // content.substr(content.find(" "))
    size_t index;
    index = content.find(" ");
    status = atoi(content.substr(index).c_str());
   
    //get max-age
    if((index = content.find("max-age")) != string::npos){
        max_age = atoi(content.substr(index + 8).c_str());
        
        
    }
    //get date
    if((index = content.find("Date: ")) != string::npos){
        string temp = content.substr(index);
        size_t start = temp.find(" ") + 1;
        size_t end = temp.find(" GMT");
        //cout << temp << endl;
        date = temp.substr(start, end - start + 1);
       
    }

    //Last-Modified: Wed, 21 Oct 2015 07:28:00 GMT
    if((index = content.find("Last-Modified: ")) != string::npos){
        string temp = content.substr(index);
        size_t start = temp.find(" ") + 1;
        size_t end = temp.find(" GMT");
        last_modified = temp.substr(start, end - start + 1);
        
    }

    //Expires: Wed, 21 Oct 2015 07:28:00 GMT
     if((index = content.find("Expires: ")) != string::npos){
        string temp = content.substr(index);
        size_t start = temp.find(" ") + 1;
        size_t end = temp.find(" GMT");
        expire = temp.substr(start, end - start + 1);
        
    }

    //ETag: "33a64df551425fcc55e4d42a148795d9f25f89d4"
     if((index = content.find("ETag: ")) != string::npos){
        string temp = content.substr(index);
        size_t start = temp.find("\"") + 1;
        temp = temp.substr(start);
        size_t end = temp.find("\"");
        etag = temp.substr(0, end);
    }

 }

