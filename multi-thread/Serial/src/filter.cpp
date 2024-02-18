#include "filter.hpp"

#include<vector>
#include<iostream>

#include "image.hpp"
#include "pixel.hpp"

void BlurFilter::apply(Image* img)
{
    std::vector<std::vector<Pixel>> originalPixels = img->getPixels();

    for(int r=0; r<img->getRows(); r++)
        for(int c=0; c<img->getCols(); c++)
            (*img)(r, c) = calcNewPixel(r, c, originalPixels);
}

Pixel BlurFilter::calcNewPixel(int row, int col, const std::vector<std::vector<Pixel>>& pixels)
{
    std::vector<std::vector<Pixel>> pixelMatrix = generatePixelMatrix(pixels, row, col);
    int newRed=0, newGreen=0, newBlue=0;

    for(int r=0; r<3; r++)
    {
        for(int c=0; c<3; c++)
        {
            newRed   += pixelMatrix[r][c].red * filter::blurKernel[r][c];
            newGreen += pixelMatrix[r][c].green * filter::blurKernel[r][c];
            newBlue  += pixelMatrix[r][c].blue * filter::blurKernel[r][c];
        }
    }

    return Pixel(newRed/filter::blurNormalizeValue, newGreen/filter::blurNormalizeValue, newBlue/filter::blurNormalizeValue);
}

std::vector<std::vector<Pixel>> BlurFilter::generatePixelMatrix(const std::vector<std::vector<Pixel>>& pixels, int row, int col)
{
    std::vector<std::vector<Pixel>> matrix;
    int maxRow = pixels.size() - 1;
    int maxCol = pixels[0].size() - 1;

    for(int r=0; r<3; r++)
    {
        matrix.push_back(std::vector<Pixel>());
        for(int c=0; c<3; c++)
        {
            matrix[r].push_back((isOutOfSize(maxCol, maxRow, row+r-1, col+c-1)) ? pixels[row][col] : pixels[row+r-1][col+c-1]);
        }
    }
    return matrix;
}

bool BlurFilter::isOutOfSize(int maxCol, int maxRow, int row, int col)
{
    return (row > maxRow) || (col > maxCol) || (col < 0) || (row < 0);
}

void FlipFilter::apply(Image* img)
{
    std::vector<std::vector<Pixel>> originalPixels = img->getPixels();
    for(int r=0; r<img->getRows(); r++)
        for(int c=0; c<img->getCols(); c++)
            (*img)(r, c) = originalPixels[img->getRows() - r - 1][c];
}

void PurpleHazeFilter::apply(Image* img)
{
    for(int r=0; r<img->getRows(); r++)
        for(int c=0; c<img->getCols(); c++)
                (*img)(r, c) = convertToPurple((*img)(r, c));
}

Pixel PurpleHazeFilter::convertToPurple(Pixel p)
{
    int r = p.red*0.5  + p.green*0.3 + p.blue*0.5;
    int g = p.red*0.16 + p.green*0.5 + p.blue*0.16;
    int b = p.red*0.6  + p.green*0.2 + p.blue*0.8;
    return Pixel(r, g, b);
}

void LineFilter::apply(Image* img)
{
    int maxCol = img->getCols() - 1;
    int maxRow = img->getRows() - 1;
    drawLine(maxCol, 0, 0, maxRow, img);
    drawLine(maxCol/2, 0, 0, maxRow/2, img);
    drawLine(maxCol/2, maxRow, maxCol, maxRow/2, img);
}

void LineFilter::drawLine(int x2, int y2, int x1, int y1, Image* img)
{
    float maxX, minX, maxY, minY;
    if(x2 > x1)
    {
        maxX = x2;
        maxY = y2;
        minX = x1;
        minY = y1;
    }
    else 
    {
        maxX = x1;
        maxY = y1;
        minX = x2;
        minY = y2;
    }
    
    float a = (maxY-minY)/(maxX-minX);

    for(int i=0; i<=(maxX-minX); i++)
    {
        (*img)(i*a+minY, i+minX) = Pixel(255, 255, 255);
    }
}
