#include "meter.hpp"

#include<string>

#include "exception.hpp"
#include "logger.hpp"
#include "consts.hpp"

int main(int argc, char* argv[])
{
    Logger* lg = new FileLogger({(std::string(argv[2])+"_"+argv[3]), consts::centeralLogFile}, consts::logPath);
    try
    {
        Meter meter(lg, argv[3], argv[1]);
        meter.start();
    }
    catch(Exception* exp)
    {
        (exp->isError) ? lg->logError(exp->msg) : lg->logInfo(exp->msg);
    }

    return 0;
}
