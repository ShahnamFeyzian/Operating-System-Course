#include "financial.hpp"

#include<string>
#include<vector>

#include<unistd.h>

#include "communication.hpp"
#include "consts.hpp"
#include "exception.hpp"
#include "logger.hpp"

using namespace std;

Financial::Financial(Logger* lg, const string& name, const vector<string>& buildingNames)
{
    lg_ = lg;
    name_ = name;
    lg_->logInfo("start to work");
    dto_ = new dto::BillConfigDto(consts::billConfigDbPath);
    setupBuildingConnections(buildingNames);
}

void Financial::setupBuildingConnections(const vector<string>& buildingNames)
{
    for(int i=0; i<buildingNames.size(); i++)
    {
        lg_->logInfo("connecting to '" + buildingNames[i] + "'");
        buildings_.push_back(new CoworkerUnit(buildingNames[i], stdmsg::coworkersDefaultId));
    }
}

void Financial::start()
{
    while(true)
    {
        vector<string> args;
        string cmd = stdmsg::sread(STDIN_FILENO, args);
        handleCmd(cmd, args);
    }
}

void Financial::handleCmd(const string& cmd, const vector<string>& args)
{
    if(cmd == stdmsg::test)
        testBuildingConnections(args);
    else if(cmd == stdmsg::closep)
        closingProcess(false);
    else if(cmd == stdmsg::billReport)
        billReport(args);
    else
        lg_->logError("unknown command");
}

void Financial::billReport(const vector<string>& args)
{
    string buildingName = args[0];
    string resource = args[1];
    string monthStr = args[2];
    int buildingIndex = findBuilding(buildingName);
    vector<int> params = dto_->getParam(stoi(monthStr));
    int finalParam;
    for(int i=0; i<3; i++)
        if(consts::resources[i] == resource) finalParam = params[i];
    
    lg_->logInfo("send parameter to building '" + buildingName + "'");
    buildings_[buildingIndex]->sendMessage(stdmsg::dataReport, {to_string(finalParam)});
}

void Financial::testBuildingConnections(const vector<string>& args)
{
    string senderName = args[0];
    string buildingName = args[1];
    lg_->logInfo("test connection with '" + buildingName + "', requested from '" + senderName + "'");
    int index = findBuilding(buildingName);
    if(index == -1) 
    {
        stdmsg::swrite(STDOUT_FILENO, string(stdmsg::nok));
        return;
    }

    lg_->logInfo("recieving test message from '" + buildingName + "'");
    string msg = buildings_[index]->recieveMessage();
    lg_->logInfo("message test connection from '" + buildingName + "': " + msg);
    lg_->logInfo("sending response to '" + buildingName + "'");
    buildings_[index]->sendMessage(stdmsg::ok);
    stdmsg::swrite(STDOUT_FILENO, string(stdmsg::ok));
}

int Financial::findBuilding(const string& name)
{
    for(int i=0; i<buildings_.size(); i++)
    {
        if(name == buildings_[i]->getName()) return i;
    }
    lg_->logError("building not found");
    return -1;
}

void Financial::closingProcess(bool isError)
{
    throw new Exception("process closed", isError);
}
