#ifndef METER_HPP
#define METER_HPP

#include<string>
#include<vector>

#include "dto.hpp"
#include "logger.hpp"

class Meter
{
public:
    Meter(Logger* lg, std::string name, std::string path);
    void start();

private:
    Logger* lg_;
    std::string name_;
    std::string dbPath_;
    dto::ResourceUsageDto* dto_;

    void handleCmd(const std::string& cmd, const std::vector<std::string>& args);
    void testConnection(const std::vector<std::string>& args);
    void closingProcess(bool isError);
    void meanReport(const std::vector<std::string>& args);
    void totalReport(const std::vector<std::string>& args);
    void maxReport(const std::vector<std::string>& args);
    void diffReport(const std::vector<std::string>& args);
    void billReport(const std::vector<std::string>& args);
};

#endif
