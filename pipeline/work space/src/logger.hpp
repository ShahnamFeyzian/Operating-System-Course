#ifndef LOGGER_HPP
#define LOGGER_HPP

#include<string>
#include<vector>

namespace lg
{
    constexpr char ERROR[] = "ERROR"; 
    constexpr char INFO [] = "INFO"; 
}

class Logger
{
public:
    Logger(const std::string& name);
    virtual void logInfo(const std::string& msg) = 0;
    virtual void logError(const std::string& msg) = 0;

protected:
    std::string name_;

};

class FileLogger : public Logger
{
public:
    FileLogger(const std::vector<std::string>& names, const std::string& path);
    virtual void logInfo(const std::string& msg);
    virtual void logError(const std::string& msg);

private:
    std::vector<std::string> paths_;
};

class ConsoleLogger : public Logger
{
public:
    ConsoleLogger(const std::string& name);
    virtual void logInfo(const std::string& msg);
    virtual void logError(const std::string& msg);

};

class FileConsoleLogger : public Logger
{
public:
    FileConsoleLogger(const std::vector<std::string>& names, const std::string& path);
    virtual void logInfo(const std::string& msg);
    virtual void logError(const std::string& msg);

private:
    Logger* consolLogger_;
    Logger* fileLogger_;

};

#endif
