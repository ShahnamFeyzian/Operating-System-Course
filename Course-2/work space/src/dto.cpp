#include "dto.hpp"

#include<fstream>
#include<string>
#include<vector>

#include <bits/stdc++.h>

using namespace std;
using namespace dto;

ResourceUsageDto::ResourceUsageDto(string dbPath)
{
    dbPath_ = dbPath;
    loadDataFromDb();
}

void ResourceUsageDto::loadDataFromDb()
{
    ifstream dbfile(dbPath_);
    string line;
    getline(dbfile, line);
    while(getline(dbfile, line))
    {
        stringstream ss(line);
        vector<string> tempParam;
        while (ss.good()) {
            string substr;
            getline(ss, substr, ',');
            tempParam.push_back(substr);
        }
        
        ResourceUsageRecord* newRecord = new ResourceUsageRecord;
        newRecord->year = tempParam[0];
        newRecord->month = atoi(tempParam[1].c_str());
        newRecord->day = atoi(tempParam[2].c_str());
        for(int i=0; i<6; i++)
        {
            newRecord->hoursUsage[i] = atoi(tempParam[i+3].c_str());
        }

        records_.push_back(newRecord);
    }
}

double ResourceUsageDto::calcMean(int month)
{
    double sum = 0;
    for(int i=0; i<records_.size(); i++)
        if(records_[i]->month == month)
            for(int j=0; j<6; j++)
                sum += records_[i]->hoursUsage[j];

    return sum/30;
}

double ResourceUsageDto::calcDiff(int month)
{
    double mean = calcMean(month) / 6;
    vector<int> max = maxHoursUsage(month);
    double totalMax = totalHourUsage(month, max[0]) / 30;
    return totalMax - mean;
}

vector<int> ResourceUsageDto::maxHoursUsage(int month)
{
    int totalUsage[6];
    for(int i=0; i<6; i++)
    {
        totalUsage[i] = 0;
        for(int j=0; j<records_.size(); j++)
        {
            if(records_[j]->month == month) totalUsage[i] += records_[j]->hoursUsage[i];
        }
    }

    vector<int> maxUsagesIndex;
    int maxUsage = 0;
    for(int i=0; i<6; i++)
    {
        if(maxUsage < totalUsage[i])
        {
            maxUsage = totalUsage[i];
            maxUsagesIndex.clear();
            maxUsagesIndex.push_back(i);
        }
        else if(maxUsage == totalUsage[i])
        {
            maxUsagesIndex.push_back(i);
        }
    }

    return maxUsagesIndex;
}

double ResourceUsageDto::totalHourUsage(int month, int hour)
{
    double sum = 0;
    for(int i=0; i<records_.size(); i++)
        if(records_[i]->month == month) sum += records_[i]->hoursUsage[hour];

    return sum;
}

double ResourceUsageDto::calcBillWater(int month, int param)
{
    double res = 0;
    vector<int> maxHours = maxHoursUsage(month);

    for(int i=0; i<records_.size(); i++)
    {
        if(records_[i]->month == month)
        {
            for(int j=0; j<6; j++)
            {
                bool isMax = false;
                for(int k=0; k<maxHours.size(); k++) 
                {
                    if(maxHours[k] == j)
                    {
                        isMax = true;
                        break;
                    }
                }
                res += (isMax) ? records_[i]->hoursUsage[j] * 1.25 : records_[i]->hoursUsage[j];
            }
        }
    }

    return res*param;
}

double ResourceUsageDto::calcBillGas(int month, int param)
{
    double res = 0;
    double totalUsage = calcMean(month) * 30;
    res = totalUsage * param;
    return res;
}

double ResourceUsageDto::calcBillElectricity(int month, int param)
{
    double res = 0;
    vector<int> maxHours = maxHoursUsage(month);
    double mean = calcMean(month) / 6;

    for(int i=0; i<records_.size(); i++)
    {
        if(records_[i]->month == month)
        {
            for(int j=0; j<6; j++)
            {
                bool isMax = false;
                for(int k=0; k<maxHours.size(); k++) 
                {
                    if(maxHours[k] == j)
                    {
                        isMax = true;
                        break;
                    }
                }
                double husage = records_[i]->hoursUsage[j];
                res += (isMax) ? husage * 1.25 : (husage < mean) ? husage * 0.75 : husage;
            }
        }
    }

    return res*param; 
}

BillConfigDto::BillConfigDto(string dbPath)
{
    dbPath_ = dbPath;
    loadDataFromDb();
}

void BillConfigDto::loadDataFromDb()
{
    ifstream dbfile(dbPath_);
    string line;
    getline(dbfile, line);
    while(getline(dbfile, line))
    {
        stringstream ss(line);
        vector<string> tempParam;
        while (ss.good()) {
            string substr;
            getline(ss, substr, ',');
            tempParam.push_back(substr);
        }
        
        BillConfigRecord* newRecord = new BillConfigRecord;
        newRecord->year = tempParam[0];
        newRecord->month = atoi(tempParam[1].c_str());
        newRecord->water = atoi(tempParam[2].c_str());
        newRecord->gas = atoi(tempParam[3].c_str());
        newRecord->electricity = atoi(tempParam[4].c_str());
        
        records_.push_back(newRecord);
    }
}

vector<int> BillConfigDto::getParam(int month)
{
    for(int i=0; i<records_.size(); i++)
        if(records_[i]->month == month)
            return {records_[i]->water, records_[i]->gas, records_[i]->electricity};
}
