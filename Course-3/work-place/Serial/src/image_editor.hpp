#ifndef IMAGE_EDITOR_HPP
#define IMAGE_EDITOR_HPP

#include<string>
#include<vector>

#include "filter.hpp"

class ImageEditor
{
public:
    ImageEditor();
    void editImage(const std::string& inputPath, const std::string& outputPath);

private:
    std::vector<Filter*> filterPipe_;
};

#endif // IMAGE_EDITOR_HPP
