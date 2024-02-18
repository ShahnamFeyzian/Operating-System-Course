#include "logger.hpp"

#include<fstream>
#include<iostream>
#include<string>

#include "consts.hpp"

using namespace std;

Logger::Logger(const string& name)
{
    name_ = name;
}

FileLogger::FileLogger(const vector<string>& names, const string& path) : Logger(names[0])
{
    for(int i=0; i<names.size(); i++)
        paths_.push_back(path + names[i] + consts::logfileType);
}

void FileLogger::logInfo(const string& msg)
{
    for(int i=0; i<paths_.size(); i++)
    {
        fstream logFile(paths_[i], ios::out | ios::app);
        logFile << lg::INFO << " '" << name_ << "': " << msg << endl;
        logFile.close();
    }
}

void FileLogger::logError(const string& msg)
{
    for(int i=0; i<paths_.size(); i++)
    {
        fstream logFile(paths_[i], ios::out | ios::app);
        logFile << lg::ERROR << " '" << name_ << "': " << msg << endl;
        logFile.close();
    }
}

ConsoleLogger::ConsoleLogger(const string& name) : Logger(name) {}

void ConsoleLogger::logInfo(const string& msg)
{
    cout << BOLDYELLOW << "LOG " << name_ << ": " << msg << RESET << endl;
}

void ConsoleLogger::logError(const string& msg)
{
    cout << BOLDRED << "LOG " << name_ << ": " << msg << RESET << endl;
}

FileConsoleLogger::FileConsoleLogger(const vector<string>& names, const string& path) : Logger(names[0])
{
    consolLogger_ = new ConsoleLogger(names[0]);
    fileLogger_ = new FileLogger(names, path);
}

void FileConsoleLogger::logInfo(const string& msg)
{
    consolLogger_->logInfo(msg);
    fileLogger_->logInfo(msg);
}

void FileConsoleLogger::logError(const string& msg)
{
    consolLogger_->logError(msg);
    fileLogger_->logError(msg);
}
