#include "building.hpp"

#include<iostream>
#include<string>
#include<vector>

#include<unistd.h>

#include "consts.hpp"
#include "communication.hpp"
#include "exception.hpp"
#include "logger.hpp"

using namespace std;

Building::Building(Logger* lg, string name, string path)
{
    lg_ = lg;
    name_ = name;
    path_ = path;
    lg_->logInfo("start to work");
    createMeters();
    financialConnectionSetup();
}

void Building::createMeters()
{
    for(int i=0; i < consts::resourceCount; i++)
    {
        int pfds1[2], pfds2[2], pid;
        if(pipe(pfds1) == -1 || pipe(pfds2) == -1) throw new Exception("problem in creating pipe", true);
    
        pid = fork();
        if(pid == 0)
        {
            lg_->logInfo("creating meter '" + string(consts::resources[i]) + "'");
            stdmsg::pipeSetup(pfds1, pfds2);
            execl(consts::exeMeter, consts::exeMeter, string(path_+"/"+name_).c_str(), name_.c_str(), consts::resources[i], nullptr);
            throw new Exception("problem in executing meter '" + string(consts::resources[i]) + "'", true);
        }
        else if(pid > 0)
        {
            close(pfds1[0]);
            close(pfds2[1]);
            meters_.push_back(new ChildUnit(string(consts::resources[i]), pfds1[1], pfds2[0], pid));
        }
        else throw new Exception("problem in forking meters", true);
    }
}
 
void Building::financialConnectionSetup()
{
    lg_->logInfo("connecting to '" + string(consts::financialBuildingName) + "'");
    financialUnit_ = new CoworkerUnit(name_, stdmsg::coworkersDefaultId);
}

void Building::start()
{
    while(true)
    {
        vector<string> args;
        string cmd = stdmsg::sread(STDIN_FILENO, args);
        handleCmd(cmd, args);
    }
}

void Building::handleCmd(const string& cmd, const vector<string>& args)
{
    if(cmd == stdmsg::test)
        testConnections(args);
    else if(cmd == stdmsg::closep)
        closingProcess(false);
    else if(cmd == stdmsg::meanReport)
        meanReport(args);
    else if(cmd == stdmsg::totalReport)
        totalReport(args);
    else if(cmd == stdmsg::maxHoursUsageReport)
        maxReport(args);
    else if(cmd == stdmsg::differenceReport)
        diffReport(args);
    else if(cmd == stdmsg::billReport)
        billReport(args);
    else
        lg_->logError("unknown command");
}

void Building::testConnections(const vector<string>& args)
{
    lg_->logInfo("test connection request from '" + args[0] + "'");
    testMeterConnections();
    testFinancialUnitConnection();
    stdmsg::swrite(STDOUT_FILENO, string(stdmsg::ok));
}

void Building::testMeterConnections()
{
    vector<string> args = {name_};
    for(int i=0; i<meters_.size(); i++)
    {
        lg_->logInfo("test message to meter '" + meters_[i]->getName() + "'");
        meters_[i]->sendMessage(stdmsg::test, args);
        string response = meters_[i]->recieveMessage();
        lg_->logInfo("response test connection from meter '" + meters_[i]->getName() + "': " + response);
    }   
}

void Building::testFinancialUnitConnection()
{
    vector<string> args = {name_};
    lg_->logInfo("test message to '" + string(consts::financialBuildingName) + "'");
    financialUnit_->sendMessage(stdmsg::test, args);
    string response = financialUnit_->recieveMessage();
    lg_->logInfo("response test connection from '" + string(consts::financialBuildingName) + "': " + response);
}

void Building::closingProcess(bool isError)
{
    for(int i=0; i<meters_.size(); i++)
        meters_[i]->sendMessage(stdmsg::closep);
    
    throw new Exception("building closed", isError);
}

void Building::meanReport(const vector<string>& args)
{
    int index = findMeter(args[0]);
    string monthNumber = args[1];
    lg_->logInfo("send mean report request to meter '" + meters_[index]->getName() + "'");
    meters_[index]->sendMessage(stdmsg::meanReport, {monthNumber});
    vector<string> resArgs;
    meters_[index]->recieveMessage(resArgs);
    stdmsg::swrite(STDOUT_FILENO, stdmsg::response, resArgs);
}

void Building::totalReport(const vector<string>& args)
{
    int index = findMeter(args[0]);
    string monthNumber = args[1];
    lg_->logInfo("send total report request to meter '" + meters_[index]->getName() + "'");
    meters_[index]->sendMessage(stdmsg::totalReport, {monthNumber});
    vector<string> resArgs;
    meters_[index]->recieveMessage(resArgs);
    stdmsg::swrite(STDOUT_FILENO, stdmsg::response, resArgs);
}

void Building::maxReport(const vector<string>& args)
{
    int index = findMeter(args[0]);
    string monthNumber = args[1];
    lg_->logInfo("send max hours usage report request to meter '" + meters_[index]->getName() + "'");
    meters_[index]->sendMessage(stdmsg::maxHoursUsageReport, {monthNumber});
    vector<string> resArgs;
    meters_[index]->recieveMessage(resArgs);
    stdmsg::swrite(STDOUT_FILENO, stdmsg::response, resArgs);
}

void Building::diffReport(const vector<string>& args)
{
    int index = findMeter(args[0]);
    string monthNumber = args[1];
    lg_->logInfo("send difference report request to meter '" + meters_[index]->getName() + "'");
    meters_[index]->sendMessage(stdmsg::differenceReport, {monthNumber});
    vector<string> resArgs;
    meters_[index]->recieveMessage(resArgs);
    stdmsg::swrite(STDOUT_FILENO, stdmsg::response, resArgs);
}

void Building::billReport(const vector<string>& args)
{
    string meterType = args[0];
    int index = findMeter(meterType);
    string monthNumber = args[1];
    lg_->logInfo("getting bills config from '" + string(consts::financialBuildingName) + "'");
    vector<string> param;
    financialUnit_->recieveMessage(param);
    meters_[index]->sendMessage(stdmsg::billReport, {monthNumber, meterType, param[0]});
    vector<string> resArgs;
    meters_[index]->recieveMessage(resArgs);
    stdmsg::swrite(STDOUT_FILENO, stdmsg::response, resArgs);
}

int Building::findMeter(const string& name)
{
    for(int i=0; i<meters_.size(); i++)
        if(meters_[i]->getName() == name) return i;

    return -1;
}
