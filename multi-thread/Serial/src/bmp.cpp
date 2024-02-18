#include "bmp.hpp"

#include<fstream>
#include<string>
#include<vector>

#include "pixel.hpp"

Bmp::Bmp(const std::string& bmpPath)
{
    read(bmpPath);
}

void Bmp::read(const std::string& bmpPath)
{
    std::ifstream file(bmpPath);

    file.seekg(0, std::ios::end);
    std::streampos length = file.tellg();
    file.seekg(0, std::ios::beg);

    fileData_ = new char[length];
    file.read(&fileData_[0], length);

    fileHeader_ = (bmp::PBITMAPFILEHEADER)(&fileData_[0]);
    infoHeader_ = (bmp::PBITMAPINFOHEADER)(&fileData_[0] + sizeof(bmp::BITMAPFILEHEADER));
    rows_ = infoHeader_->biHeight;
    cols_ = infoHeader_->biWidth;
    size_ = fileHeader_->bfSize;

    file.close();
}

std::vector<std::vector<Pixel>> Bmp::generatePixelMatrix()
{
    std::vector<std::vector<Pixel>> pixels;
    for(int r=0; r<rows_; r++)
    {
        pixels.push_back(std::vector<Pixel>());
        for(int c=0; c<cols_; c++)
        {
            pixels[r].push_back(getPixel(r, c));
        }
    }
    return pixels;
}

int Bmp::calcPadding(int width)
{
    int bytesInRow = width * sizeof(Pixel);
    return bytesInRow % 4 ? 4 - bytesInRow % 4 : 0;
}

Pixel& Bmp::getPixel(int row, int col) {
    int rowStart = (rows_ - 1 - row) * (cols_ * sizeof(Pixel) + padding_);
    char* pixelPos = fileData_ + fileHeader_->bfOffBits + rowStart + col * sizeof(Pixel);
    return *(Pixel*)pixelPos;
}

void Bmp::write(const std::string& bmpPath,const std::vector<std::vector<Pixel>>& matrix)
{
    for (int r = 0; r < rows_; ++r) 
        for (int c = 0; c < cols_; ++c)
            getPixel(r, c) = matrix[r][c];

    std::ofstream file(bmpPath, std::ios_base::binary);
    file.write(fileData_, fileHeader_->bfSize);
}

int Bmp::getRows()
{
    return rows_;
}

int Bmp::getCols()
{
    return cols_;
}
