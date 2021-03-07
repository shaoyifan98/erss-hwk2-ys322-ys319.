#ifndef _MYEXCEPTION_H
#define _MYEXCEPTION_H

#include <exception>
#include <string>

class myException: public std::exception {
  private:
    std::string errorMsg;
  public:
	  myException(std::string str): errorMsg(str) {}
    const char * what() const throw(){
        return errorMsg.c_str();
    }
};

#endif