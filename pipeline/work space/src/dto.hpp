#ifndef DTOS_HPP
#define DTOS_HPP

#include<string>
#include<vector>

namespace dto
{
struct ResourceUsageRecord
{
    std::string year;
    int month;
    int day;
    int hoursUsage[6];
};

struct BillConfigRecord
{
    std::string year;
    int month;
    int water;
    int gas;
    int electricity;
};

class ResourceUsageDto
{
    public:
    ResourceUsageDto(std::string dbPath);
    double calcMean(int month);
    double calcDiff(int month);
    double calcBillWater(int month, int param);
    double calcBillGas(int month, int param);
    double calcBillElectricity(int month, int param);
    double totalHourUsage(int month, int hour);
    std::vector<int> maxHoursUsage(int month);

    private:
    std::string dbPath_;
    std::vector<ResourceUsageRecord*> records_;

    void loadDataFromDb();

};

class BillConfigDto
{
    public:
    BillConfigDto(std::string dbPath);
    std::vector<int> getParam(int month);

    private:
    std::string dbPath_;
    std::vector<BillConfigRecord*> records_;

    void loadDataFromDb();
};
}

#endif
