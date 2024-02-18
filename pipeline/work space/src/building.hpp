#ifndef BUILDING_HPP
#define BUILDING_HPP

#include<string>
#include<vector>

#include "communication.hpp"
#include "logger.hpp"

class Building
{
public:
    Building(Logger* lg, std::string name, std::string path);
    void start();

private:
    Logger* lg_;
    std::string name_;
    std::string path_;
    std::vector<CommunicationUnit*> meters_;
    CommunicationUnit* financialUnit_;

    void createMeters();
    void financialConnectionSetup();
    void handleCmd(const std::string& cmd, const std::vector<std::string>& args);
    void testConnections(const std::vector<std::string>& args);
    void testMeterConnections();
    void testFinancialUnitConnection();
    void closingProcess(bool isError);
    int findMeter(const std::string& name);
    void meanReport(const std::vector<std::string>& args);
    void totalReport(const std::vector<std::string>& args);
    void maxReport(const std::vector<std::string>& args);
    void diffReport(const std::vector<std::string>& args);
    void billReport(const std::vector<std::string>& args);
};

#endif
