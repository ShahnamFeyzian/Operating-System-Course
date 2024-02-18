#include "company.hpp"

#include <unistd.h>

#include "consts.hpp"
#include "exception.hpp"
#include "logger.hpp"
#include "utils.hpp"

#include<iostream>

void initialEnvironment();

int main(int argc, char* argv[])
{
    Logger* lg = new FileConsoleLogger({consts::companyName, consts::centeralLogFile}, consts::logPath); 
    try
    {
        initialEnvironment();
        Company company(argv[1], lg);
        sleep(1);
        company.testCommunications();
        company.start();
    }
    catch(Exception* exp)
    {
        (exp->isError) ? lg->logError(exp->msg) : lg->logInfo(exp->msg);
    }
    
    return 0;
}

void initialEnvironment()
{
    int stat1 = utils::createDirectory(consts::fifoPath);
    int stat2 = utils::createDirectory(consts::logPath);
    if(stat1 == -1 || stat2 == -1) throw new Exception("problem in initializing environment", true);
}
