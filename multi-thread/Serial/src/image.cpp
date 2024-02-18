#include "image.hpp"

#include<string>
#include<vector>

#include "bmp.hpp"
#include "pixel.hpp"

void Image::readBmp(const std::string& filePath)
{
    bmp = new Bmp(filePath);
    pixels_ = bmp->generatePixelMatrix();
}

void Image::writeBmp(const std::string& filePath)
{
    bmp->write(filePath, pixels_);
}

std::vector<std::vector<Pixel>> Image::getPixels()
{
    return pixels_;
}

int Image::getRows()
{
    return bmp->getRows();
}

int Image::getCols()
{
    return bmp->getCols();
}

Pixel& Image::operator()(int row, int col)
{
    return pixels_[row][col];
}

Pixel Image::operator()(int row, int col) const
{
    return pixels_[row][col];
}
