#include "building.hpp"

#include "exception.hpp"
#include "logger.hpp"
#include "consts.hpp"

using namespace std;

int main(int argc, char* argv[])
{
    Logger* lg = new FileLogger({argv[2], consts::centeralLogFile}, consts::logPath);
    try
    {
        Building building(lg, argv[2], argv[1]);
        building.start();
    }
    catch(Exception* exp)
    {
        (exp->isError) ? lg->logError(exp->msg) : lg->logInfo(exp->msg);
    }
    
    return 0;
}
