#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include<string>

class Exception
{
public:
    Exception(std::string msg, bool isError) {this->msg = msg; this->isError = isError;}
    std::string msg;
    bool isError;

};

#endif