#ifndef UTILS_HPP
#define UTILS_HPP

#include<string>
#include<vector>

namespace utils
{
    std::vector<std::string> getDirNames(const char* path);
    char** generateArgs(const std::vector<std::string>& inArgs);
    std::vector<std::string> vectorFromArgs(char** argsIn, int start, int end);
    int createDirectory(const std::string& path);
}

#endif
