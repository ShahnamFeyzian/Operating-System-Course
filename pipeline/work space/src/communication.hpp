#ifndef COMMUNICATION_HPP
#define COMMUNICATION_HPP

#include<string>
#include<vector>

namespace stdmsg
{
    constexpr int coworkersDefaultId = 546;
    constexpr int bufSize = 1024;

    constexpr char ok    [] = "OK";
    constexpr char nok   [] = "problem";
    constexpr char test  [] = "test";
    constexpr char closep[] = "close";
    constexpr char help  [] = "help";    
    constexpr char report[] = "report";    
    constexpr char response[] = "response";    
    constexpr char meanReport[] = "mean";    
    constexpr char totalReport[] = "total";    
    constexpr char maxHoursUsageReport[] = "max";    
    constexpr char differenceReport[] = "difference";    
    constexpr char billReport[] = "bill";    
    constexpr char dataReport[] = "data";    

    void pipeSetup(int* fds1, int* fds2);
    void swrite(int fd, const std::string& cmd);
    void swrite(int fd, const std::string& cmd, const std::vector<std::string>& args);
    std::string sread(int fd);
    std::string sread(int fd, std::vector<std::string>& args);
    std::string serilizeStdmsg(const std::string& cmd, const std::vector<std::string>& args);
    std::string parseStdmsg(const std::string& msg, std::vector<std::string>& args);
}

class CommunicationUnit
{
public:
    CommunicationUnit(const std::string& name, int id);
    virtual void sendMessage(const std::string& cmd) = 0;
    virtual void sendMessage(const std::string& msg, const std::vector<std::string>& args) = 0;
    virtual std::string recieveMessage() = 0;
    virtual std::string recieveMessage(std::vector<std::string>& args) = 0;
    std::string getName();

protected:
    std::string name_;
    int id_;
};

class ChildUnit : public CommunicationUnit
{
public:
    ChildUnit(const std::string& name, int writefd, int readfd, int id);
    virtual void sendMessage(const std::string& cmd);
    virtual void sendMessage(const std::string& cmd, const std::vector<std::string>& args);
    virtual std::string recieveMessage();
    virtual std::string recieveMessage(std::vector<std::string>& args);

private:
    int writefd_;
    int readfd_;

};

class CoworkerUnit : public CommunicationUnit
{
public:
    CoworkerUnit(const std::string& name, int id);
    virtual void sendMessage(const std::string& cmd);
    virtual void sendMessage(const std::string& cmd, const std::vector<std::string>& args);
    virtual std::string recieveMessage();
    virtual std::string recieveMessage(std::vector<std::string>& args);

private:
    std::string fifoPath_;

};

#endif
