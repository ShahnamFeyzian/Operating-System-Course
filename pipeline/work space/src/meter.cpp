#include "meter.hpp"

#include<string>

#include<unistd.h>

#include "exception.hpp"
#include "consts.hpp"
#include "communication.hpp"
#include "logger.hpp"

using namespace std;

Meter::Meter(Logger* lg, string name, string path)
{
    lg_ = lg;
    name_ = name;
    dbPath_ = path + "/" + name + consts::databaseType;
    lg_->logInfo("start to work");
    dto_ = new dto::ResourceUsageDto(dbPath_);
}

void Meter::start()
{
    while(true)
    {
        vector<string> args;
        string cmd = stdmsg::sread(STDIN_FILENO, args);
        handleCmd(cmd, args);
    }
}

void Meter::handleCmd(const string& cmd, const vector<string>& args)
{
    if(cmd == stdmsg::test)
        testConnection(args);
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

void Meter::testConnection(const vector<string>& args)
{
    string senderName = args[0];
    lg_->logInfo("test connection request from '" + senderName + "'");
    stdmsg::swrite(STDOUT_FILENO, string(stdmsg::ok));
}

void Meter::closingProcess(bool isError)
{
    throw new Exception("process closed", isError);
}

void Meter::meanReport(const vector<string>& args)
{
    int month = stoi(args[0]);
    double res = dto_->calcMean(month);
    stdmsg::swrite(STDOUT_FILENO, stdmsg::response, {to_string(res)});
}

void Meter::totalReport(const vector<string>& args)
{
    int month = stoi(args[0]);
    double res = dto_->calcMean(month) * 30;
    stdmsg::swrite(STDOUT_FILENO, stdmsg::response, {to_string(res)});
}

void Meter::maxReport(const vector<string>& args)
{
    int month = stoi(args[0]);
    vector<int> tempRes = dto_->maxHoursUsage(month);
    vector<string> res;
    
    for(int i=0; i<tempRes.size();  i++)
        res.push_back(to_string(tempRes[i]));
    
    stdmsg::swrite(STDOUT_FILENO, stdmsg::response, res);
}

void Meter::diffReport(const vector<string>& args)
{
    int month = stoi(args[0]);
    double res = dto_->calcDiff(month);
    stdmsg::swrite(STDOUT_FILENO, stdmsg::response, {to_string(res)});
}

void Meter::billReport(const vector<string>& args)
{
    int month = stoi(args[0]);
    string meterType = args[1];
    int param = stoi(args[2]);
    double res;
    
    if(meterType == consts::resources[0]) res = dto_->calcBillWater(month, param);
    else if(meterType == consts::resources[1]) res = dto_->calcBillGas(month, param);
    else res = dto_->calcBillElectricity(month, param);

    stdmsg::swrite(STDOUT_FILENO, stdmsg::response, {to_string(res)});
}
