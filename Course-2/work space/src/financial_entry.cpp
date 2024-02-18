#include "financial.hpp"

#include "consts.hpp"
#include "exception.hpp"
#include "logger.hpp"
#include "utils.hpp"

using namespace std;

int main(int argc, char* argv[])
{
    Logger* lg = new FileLogger({string(argv[1]), consts::centeralLogFile}, consts::logPath);
    try
    {
        Financial financial(lg, argv[1], utils::vectorFromArgs(argv, 2, argc-1));
        financial.start();
    }
    catch(Exception* exp)
    {
        (exp->isError) ? lg->logError(exp->msg) : lg->logInfo(exp->msg);
    }
    
    return 0;
}


