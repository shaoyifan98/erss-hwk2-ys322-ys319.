#include "Time.h"
#include <iostream>
#include <string.h>
// int main(){
//     Time myTime;
//     tm time = myTime.parseTime("Wed, 21 Oct 2015 07:28:00");
//     cout << " parm  time is" << asctime(&time) << endl;
//     cout << myTime.computeTimeDiff("Wed, 21 Oct 2015 07:28:00") << endl;

// }

void Time::setup(){
    month.insert(pair<std::string, int>("Jan", 0));
    month.insert(pair<std::string, int>("Feb", 1));
    month.insert(pair<std::string, int>("Mar", 2));
    month.insert(pair<std::string, int>("Apr", 3));
    month.insert(pair<std::string, int>("May", 4));
    month.insert(pair<std::string, int>("Jun", 5));
    month.insert(pair<std::string, int>("Jul", 6));
    month.insert(pair<std::string, int>("Aug", 7));
    month.insert(pair<std::string, int>("Sep", 8));
    month.insert(pair<std::string, int>("Oct", 9));
    month.insert(pair<std::string, int>("Nov", 10));
    month.insert(pair<std::string, int>("Dec", 11));
}

//Wed, 21 Oct 2015 07:28:00
struct tm Time::parseTime(string str){
    struct tm time;
    time.tm_mday = atoi(str.substr(str.find(" ")).c_str());
    time.tm_mon = month[str.substr(8, 3)];
    time.tm_year = atoi(str.substr(12, 4).c_str()) - 1900;
    strptime(str.substr(17).c_str(), "%H:%M:%S", &time);
    return time;
}

//get the current time in GMT
struct tm Time::getCurrentTime(){
    time_t timep;
    time(&timep);
    return *gmtime(&timep);
}

//get the current time in GMT in string literal
string Time::getCurrentTimeStr() {
    struct tm time = getCurrentTime();
    return string(asctime(&time));
}

//return tm1 - tm2
double Time::computeTimeDiff(tm& tm1, tm& tm2){
    time_t time1,time2;
    time1 = mktime(&tm1);
    time2 = mktime(&tm2);
    return difftime(time1,time2);
}

//return tm1 - tm2
double Time::computeTimeDiff(string str1, string str2){
    tm tm1 = parseTime(str1);
    tm tm2 = parseTime(str2);
    return computeTimeDiff(tm1, tm2);
}

//return current time - str
double Time::computeTimeDiff(string str){
    cout << "parm time str is" << str << endl;
    tm tm1 = parseTime(str);
    tm tm2 = getCurrentTime();
    cout << " current time is" << asctime(&tm2) << endl;
    cout << " parm  time is" << asctime(&tm1) << endl;
    return computeTimeDiff(tm2, tm1);
}



// int main(){
//     Time mytime;
//     string str1 = "Wed, 22 Oct 2015 08:28:00";
//     string str2 = "Wed, 22 Oct 2015 07:28:00";
//     cout << mytime.computeTimeDiff(str1, str2)<< endl;
//     cout << "Time Now: " << mytime.getCurrentTimeStr() << endl;
// }