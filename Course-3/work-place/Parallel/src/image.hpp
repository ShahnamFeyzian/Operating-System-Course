#ifndef IMAGE_HPP
#define IMAGE_HPP

#include<string>
#include<vector>

#include "bmp.hpp"
#include "pixel.hpp"

class Image
{
public:
    Image(){};
    void writeBmp(const std::string& filePath);
    void readBmp(const std::string& filePath);
    Pixel& operator()(int row, int col);
    Pixel operator()(int row, int col) const;
    std::vector<std::vector<Pixel>> getPixels();
    int getRows();
    int getCols();
private:
    Bmp* bmp = nullptr;
    std::vector<std::vector<Pixel>> pixels_;
};

#endif //IMAGE_HPP