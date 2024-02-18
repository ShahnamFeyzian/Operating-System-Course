#ifndef BMP_HPP
#define BMP_HPP

#include<string>
#include<vector>

#include "pixel.hpp"

namespace bmp
{

typedef int LONG;
typedef unsigned short WORD;
typedef unsigned int DWORD;

#pragma pack(push, 1)
typedef struct tagBITMAPFILEHEADER {
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;
#pragma pack(pop)

} //namespace bmp

class Bmp
{
public:
    Bmp(const std::string& bmpPath);
    ~Bmp();
    std::vector<std::vector<Pixel>> generatePixelMatrix();
    Pixel& getPixel(int row, int col);
    void read(const std::string& bmpPath);
    void write(const std::string& bmpPath,const std::vector<std::vector<Pixel>>& matrix);

    int getRows();
    int getCols();
private:
    int rows_;
    int cols_;
    int size_;
    int padding_;
    char* fileData_;
    bmp::PBITMAPFILEHEADER fileHeader_;
    bmp::PBITMAPINFOHEADER infoHeader_;

    int calcPadding(int width);
};

#endif //BMP_HPP
