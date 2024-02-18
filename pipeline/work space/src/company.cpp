#include "company.hpp"

#include<string>
#include<vector>
#include<iostream>

#include<bits/stdc++.h> 
#include<unistd.h>

#include "consts.hpp"
#include "communication.hpp"
#include "exception.hpp"
#include "logger.hpp"
#include "utils.hpp"

using namespace std;

Company::Company(const char* buildingsPath, Logger* lg)
{
    lg_ = lg;
    lg_->logInfo("start to work");
    vector<string> buildingNames = getBuildings(buildingsPath);
    createBuildings(buildingNames, buildingsPath);
    createFinancialUnit(buildingNames);
}

vector<string> Company::getBuildings(const char* path)
{
    vector<string> names = utils::getDirNames(path);
    if(names.size() == 0) throw new Exception("problem in buildings directory", true);
    buildingsLogge(names);
    return names;
}

void Company::createBuildings(const vector<string>& names, const char* buildingsPath)
{   
    for(int i=0; i < names.size(); i++)
    {
        int pfds1[2], pfds2[2], pid;
        if(pipe(pfds1) == -1 || pipe(pfds2) == -1) throw new Exception("problem in creating pipe", true);

        pid = fork();
        if(pid == 0)
        {
            lg_->logInfo("creating building '" + names[i] + "'");
            stdmsg::pipeSetup(pfds1, pfds2);
            execl(consts::exeBuilding, consts::exeBuilding, buildingsPath, names[i].c_str(), nullptr);
            throw new Exception("problem in executing building '" + names[i] + "'", true);
        }
        else if(pid > 0)
        {
            close(pfds1[0]);
            close(pfds2[1]);
            buildings_.push_back(new ChildUnit(names[i], pfds1[1], pfds2[0], pid));
        }
        else throw new Exception("problem in forking buildings", true);
    }
}

void Company::createFinancialUnit(const vector<string>& buildingNames)
{
    lg_->logInfo("create '" + string(consts::financialBuildingName) + "'");
    
    int pfds1[2], pfds2[2], pid;
    if(pipe(pfds1) == -1 || pipe(pfds2) == -1) throw new Exception("problem in creating pipe", true);

    pid = fork();
    if(pid == 0)
    {
        vector<string> tempArgs = {string(consts::exeFinancial), string(consts::financialBuildingName)};
        tempArgs.insert(tempArgs.end(), buildingNames.begin(), buildingNames.end());
        char** args = utils::generateArgs(tempArgs);
        stdmsg::pipeSetup(pfds1, pfds2);
        execv(consts::exeFinancial, args);
        throw new Exception("problem in executing building '" + string(consts::financialBuildingName) + "'", true);
    }
    else if(pid > 0)
    {
        close(pfds1[0]);
        close(pfds2[1]);
        financialUnit_ = new ChildUnit(consts::financialBuildingName, pfds1[1], pfds2[0], pid);
    }
    else throw new Exception("problem in forking buildings", true);
}

void Company::buildingsLogge(const vector<string>& names)
{
    string msg = "there is/are " + to_string(names.size()) + " building(s)";
    lg_->logInfo(msg);

    for(int i=0; i < names.size(); i++)
    {
        msg = to_string(i+1) + "-" + names[i];
        lg_->logInfo(msg);
    }
}

void Company::testCommunications()
{
    lg_->logInfo("start testing communications");

    for(int i=0; i<buildings_.size(); i++)
    {
        vector<string> args = {consts::companyName, buildings_[i]->getName()};
        lg_->logInfo("test message to building '" + buildings_[i]->getName() + "'");
        buildings_[i]->sendMessage(stdmsg::test, args);
        lg_->logInfo("test connection message to '" + financialUnit_->getName() + "' with '" + buildings_[i]->getName() + "'");
        financialUnit_->sendMessage(stdmsg::test, args);
        string response = buildings_[i]->recieveMessage();
        lg_->logInfo("response test connection from building '" + buildings_[i]->getName() + "': " + response);
        response = financialUnit_->recieveMessage();
        lg_->logInfo("response test connection with '" + buildings_[i]->getName() + "'from '" + financialUnit_->getName() + "': " + response);
    }
}

void Company::start()
{
    showHelpFile();
    while(true)
    {
        cout << ">> ";
        char msg[stdmsg::bufSize];
        vector<string> args;
        cin.getline(msg, stdmsg::bufSize);
        string cmd = stdmsg::parseStdmsg(string(msg), args);
        handleCmd(cmd, args);
    }
}

void Company::handleCmd(string& cmd, const vector<string>& args)
{
    if(cmd == stdmsg::test) 
        testCommunications();
    else if(cmd == stdmsg::help) 
        showHelpFile();
    else if(cmd == stdmsg::closep) 
        closingProcess(false);
    else if(cmd == stdmsg::report) 
        reportHandler(args);
    else
        lg_->logError("unknown command");
}

void Company::closingProcess(bool isError)
{
    for(int i=0; i<buildings_.size(); i++)
        buildings_[i]->sendMessage(stdmsg::closep);

    financialUnit_->sendMessage(stdmsg::closep);

    throw new Exception("process closed", isError);
}

void Company::showHelpFile()
{
    string cmd = "cat " + string(consts::helpFilePath);
    system(cmd.c_str());
}

void Company::reportHandler(const vector<string>& args)
{
    string type = args[0];
    int index = findBuilding(args[1]);
    string resourceName = args[2];
    string monthNumber = args[3];
    if(index == -1) 
    {
        lg_->logError("building not found");
        return;
    }

    if(type == stdmsg::meanReport) 
        meanReport({resourceName, monthNumber}, index);
    else if(type == stdmsg::totalReport) 
        totalReport({resourceName, monthNumber}, index);
    else if(type == stdmsg::totalReport) 
        totalReport({resourceName, monthNumber}, index);
    else if(type == stdmsg::maxHoursUsageReport) 
        maxReport({resourceName, monthNumber}, index);
    else if(type == stdmsg::differenceReport) 
        diffReport({resourceName, monthNumber}, index);
    else if(type == stdmsg::billReport) 
        billReport({resourceName, monthNumber}, index);
    else  
        lg_->logError("report type not found");
}

void Company::meanReport(const vector<string>& args, int index)
{
    lg_->logInfo("send mean report request to building '" + buildings_[index]->getName() + "'");
    vector<string> resArgs;
    buildings_[index]->sendMessage(stdmsg::meanReport, args);
    string res = buildings_[index]->recieveMessage(resArgs);
    cout << "mean report for " + args[0] + " in building " + buildings_[index]->getName() + " in month " + args[1] + " is: " + resArgs[0] + "\n";
}

void Company::totalReport(const vector<string>& args, int index)
{
    lg_->logInfo("send total report request to building '" + buildings_[index]->getName() + "'");
    vector<string> resArgs;
    buildings_[index]->sendMessage(stdmsg::totalReport, args);
    string res = buildings_[index]->recieveMessage(resArgs);
    cout << "total report for " + args[0] + " in building " + buildings_[index]->getName() + " in month " + args[1] + " is: " + resArgs[0] + "\n";
}

void Company::maxReport(const vector<string>& args, int index)
{
    lg_->logInfo("send max hours usage report request to building '" + buildings_[index]->getName() + "'");
    vector<string> resArgs;
    buildings_[index]->sendMessage(stdmsg::maxHoursUsageReport, args);
    string res = buildings_[index]->recieveMessage(resArgs);
    
    string maxHoursStr = "";
    for(int i=0; i<resArgs.size(); i++)
        maxHoursStr += (resArgs[i] + " ");

    cout << "max hours usage report for " + args[0] + " in building " + buildings_[index]->getName() + " in month " + args[1] + " is/are: " + maxHoursStr + "\n";
}

void Company::diffReport(const vector<string>& args, int index)
{
    lg_->logInfo("send difference report request to building '" + buildings_[index]->getName() + "'");
    vector<string> resArgs;
    buildings_[index]->sendMessage(stdmsg::differenceReport, args);
    string res = buildings_[index]->recieveMessage(resArgs);
    cout << "difference report for " + args[0] + " in building " + buildings_[index]->getName() + " in month " + args[1] + " is: " + resArgs[0] + "\n";
}

void Company::billReport(const vector<string>& args, int index)
{
    lg_->logInfo("send bill report request to building '" + buildings_[index]->getName() + "' and '" + consts::financialBuildingName + "'");
    vector<string> resArgs;
    buildings_[index]->sendMessage(stdmsg::billReport, args);
    vector<string> financArgs = {buildings_[index]->getName()};
    financArgs.insert(financArgs.end(), args.begin(), args.end());
    financialUnit_->sendMessage(stdmsg::billReport, financArgs);
    string res = buildings_[index]->recieveMessage(resArgs);
    cout << "bill report for " + args[0] + " in building " + buildings_[index]->getName() + " in month " + args[1] + " is: " + resArgs[0] + "\n";
}

int Company::findBuilding(const string& name)
{
    for(int i=0; i<buildings_.size(); i++)
        if(buildings_[i]->getName() == name) return i;

    return -1;
}
