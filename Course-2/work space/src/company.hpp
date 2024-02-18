#ifndef COMPANY_HPP
#define COMPANY_HPP

#include<string>
#include<vector>

#include "communication.hpp"
#include "logger.hpp"

class Company
{
public:
    Company(const char* buildingsPath, Logger* lg);
    void testCommunications();
    void start();

private:
    Logger* lg_;
    std::vector<CommunicationUnit*> buildings_;
    CommunicationUnit* financialUnit_;

    std::vector<std::string> getBuildings(const char* path);
    void createBuildings(const std::vector<std::string>& names, const char* buildingsPath);
    void createFinancialUnit(const std::vector<std::string>& buildingNames);
    void buildingsLogge(const std::vector<std::string>& names);
    void handleCmd(std::string& cmd, const std::vector<std::string>& args);
    void closingProcess(bool isError);
    void showHelpFile();
    void reportHandler(const std::vector<std::string>& args);
    void temp(const std::vector<std::string> &args, int index);
    int findBuilding(const std::string &name);
    void meanReport(const std::vector<std::string> &args, int index);
    void totalReport(const std::vector<std::string> &args, int index);
    void maxReport(const std::vector<std::string> &args, int index);
    void diffReport(const std::vector<std::string> &args, int index);
    void billReport(const std::vector<std::string> &args, int index);
};

#endif
