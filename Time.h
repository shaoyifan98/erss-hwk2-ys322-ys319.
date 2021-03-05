#ifndef TIME
#define TIME
#include <string>
#include <map>
#include <time.h>
#include <stdlib.h>

using namespace std;

class Time{
    public:
        Time(){
            setup();
        }
        map<string, int> month;
        void setup();
        struct tm parseTime(string str);
        struct tm getCurrentTime();
        string getCurrentTimeStr();
        //return the time difference, str1 - str2
        //must in http time style:"Wed, 22 Oct 2015 08:28:00"
        double computeTimeDiff(string str1, string str2);
        //compare str with current time;
        double computeTimeDiff(string str);
        double computeTimeDiff(tm& tm1, tm& tm2);
};
#endif