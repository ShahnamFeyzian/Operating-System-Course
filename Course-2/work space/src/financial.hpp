#ifndef FINANCIAL_HPP
#define FINANCIAL_HPP

#include<string>
#include<vector>

#include "communication.hpp"
#include "dto.hpp"
#include "logger.hpp"

class Financial
{
public:
    Financial(Logger* lg, const std::string& name, const std::vector<std::string>& buildingNames);
    void start();

private:
    Logger* lg_;
    std::string name_;
    std::vector<CommunicationUnit*> buildings_;
    dto::BillConfigDto* dto_;

    void setupBuildingConnections(const std::vector<std::string>& buildingNames);
    void handleCmd(const std::string& cmd, const std::vector<std::string>& args);
    void testBuildingConnections(const std::vector<std::string>& args);
    int findBuilding(const std::string& name);
    void closingProcess(bool isError);
    void billReport(const std::vector<std::string>& args);
};

#endif
