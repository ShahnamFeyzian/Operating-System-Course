#include "communication.hpp"

#include<string>

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <bits/stdc++.h>

#include "consts.hpp"

using namespace std;

namespace stdmsg
{
void pipeSetup(int* fds1, int* fds2)
{
    close(fds1[1]);
    close(fds2[0]);
    dup2(fds1[0], STDIN_FILENO);
    dup2(fds2[1], STDOUT_FILENO);
    close(fds1[0]);
    close(fds2[1]);
}

void swrite(int fd, const string& cmd)
{
    write(fd, (cmd+'\0').c_str(), cmd.size()+1);
}

void swrite(int fd, const string& cmd, const vector<string>& args)
{
    string msg = serilizeStdmsg(cmd, args);
    write(fd, (msg+'\0').c_str(), msg.size()+1);
}

string sread(int fd)
{
    vector<string> temp;
    return sread(fd, temp);
}

string sread(int fd, vector<string>& args)
{
    char buf[bufSize];
    read(fd, buf, bufSize);
    string cmd = parseStdmsg(string(buf), args);
    return cmd;
}

string serilizeStdmsg(const string& cmd, const vector<string>& args)
{
    string msg = cmd;
    for(int i=0; i<args.size(); i++)
        msg += (" " + args[i]);

    return msg;
}

string parseStdmsg(const string& msg, vector<string>& args)
{
    stringstream ss(msg);
    args.clear();
    while (ss.good()) {
        string substr;
        getline(ss, substr, ' ');
        args.push_back(substr);
    }

    string cmd = args[0];
    args.erase(args.begin());
    return cmd;
}
}

CommunicationUnit::CommunicationUnit(const string& name, int id)
{
    id_ = id;
    name_ = name;
}

ChildUnit::ChildUnit(const string& name, int writefd, int readfd, int id) : CommunicationUnit(name, id)
{
    writefd_ = writefd;
    readfd_ = readfd;
}

void ChildUnit::sendMessage(const string& cmd)
{
    write(writefd_, cmd.c_str()+'\0', cmd.size()+1);
}

void ChildUnit::sendMessage(const string& cmd, const vector<string>& args)
{
    string msg = stdmsg::serilizeStdmsg(cmd, args);
    write(writefd_, msg.c_str()+'\0', msg.size()+1);
}

string ChildUnit::recieveMessage()
{
    vector<string> temp;
    return recieveMessage(temp);
}

string ChildUnit::recieveMessage(vector<string>& args)
{
    char buf[stdmsg::bufSize];
    read(readfd_, buf, stdmsg::bufSize);
    string cmd = stdmsg::parseStdmsg(string(buf), args);
    return cmd;
}

CoworkerUnit::CoworkerUnit(const string& name, int id) : CommunicationUnit(name, id)
{
    fifoPath_ = consts::fifoPath + name;
    mkfifo(fifoPath_.c_str(), 0666);
}

void CoworkerUnit::sendMessage(const string& cmd)
{
    int fd = open(fifoPath_.c_str(),O_WRONLY);
    write(fd, cmd.c_str()+'\0', cmd.size()+1);
    close(fd);
}

void CoworkerUnit::sendMessage(const string& cmd, const vector<string>& args)
{
    string msg = stdmsg::serilizeStdmsg(cmd, args);
    int fd = open(fifoPath_.c_str(),O_WRONLY);
    write(fd, msg.c_str()+'\0', msg.size()+1);
    close(fd);
}

string CoworkerUnit::recieveMessage()
{
    vector<string> temp;
    return recieveMessage(temp);
}

string CoworkerUnit::recieveMessage(vector<string>& args)
{
    int fd = open(fifoPath_.c_str(),O_RDONLY);
    char buf[stdmsg::bufSize];
    read(fd, buf, stdmsg::bufSize);
    close(fd);
    string cmd = stdmsg::parseStdmsg(buf, args);
    return cmd;
}

string CommunicationUnit::getName() {return name_;}
