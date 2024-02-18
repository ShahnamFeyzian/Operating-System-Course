#include "utils.hpp"

#include<string>
#include<vector>
#include<iostream>

#include<sys/stat.h>
#include<dirent.h>

using namespace std;

namespace utils
{
    vector<string> getDirNames(const char* path)
    {
        vector<string> names;
    
        DIR *dir = opendir(path);
        if(!dir) return names;
        struct dirent *entry = readdir(dir);
        while (entry != NULL)
        {
            if (entry->d_type == DT_DIR)
                names.push_back(entry->d_name);

            entry = readdir(dir);
        }
        closedir(dir);

        for(int i=0; i < names.size(); i++)
        {
            if(names[i] == "." || names[i] == "..") names.erase(names.begin() + i--);
        }

        return names;
    }

    char** generateArgs(const std::vector<std::string>& inArgs)
    {
        char** outArgs = (char**)malloc((inArgs.size()+1) * sizeof(char*));

        for(int i=0; i<inArgs.size(); i++)
        {
            outArgs[i] = (char*)malloc((inArgs[i].size()+1) * sizeof(char));
            for(int j=0; j<inArgs[i].size(); j++)
            {
                outArgs[i][j] = inArgs[i][j];
            }
            outArgs[i][inArgs[i].size()] = '\0';
        }
        outArgs[inArgs.size()] = nullptr;

        return outArgs;
    }

    std::vector<std::string> vectorFromArgs(char** argsIn, int start, int end)
    {
        vector<string> res;
        for(int i = start; i <= end; i++)
        {
            res.push_back(argsIn[i]);
        }
        return res;
    }

    int createDirectory(const string& path)
    {
        struct stat sb;

        if(stat(path.c_str(), &sb) == 0)
        {
            struct dirent *ent;
            DIR *dir = opendir(path.c_str());
            if (dir != NULL) 
            {
                while ((ent = readdir(dir)) != NULL) 
                {
                    std::remove((path + ent->d_name).c_str());
                }
                closedir (dir);
            } 
            else return -1;
        }
        else
        {
            if(mkdir(path.c_str(), 0777)) return -1;
        }
        return 0;
    }
}
