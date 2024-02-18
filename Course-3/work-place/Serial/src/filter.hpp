#ifndef FILTER_HPP
#define FILTER_HPP

#include "image.hpp"

namespace filter
{

constexpr int blurKernel[3][3]    = {{ 1,  2,  1},
                                     { 2,  4,  2}, 
                                     { 1,  2,  1}};
constexpr int blurNormalizeValue = 16;

} // namespace filter


class Filter
{
public:
    Filter(){};
    virtual void apply(Image* img) = 0;
    virtual char* getName() = 0;

};

class BlurFilter : public Filter
{
public:
    BlurFilter(){};
    void apply(Image* img);
    virtual char* getName() {return "BlureFilter";}

private:
    Pixel calcNewPixel(int row, int col, const std::vector<std::vector<Pixel>>& pixels);
    std::vector<std::vector<Pixel>> generatePixelMatrix(const std::vector<std::vector<Pixel>>& pixels, int row, int col);
    bool isOutOfSize(int maxCol, int maxRow, int row, int col);
};

class FlipFilter : public Filter
{
public:
    FlipFilter(){};
    void apply(Image* img);
    virtual char* getName() {return "FlipFilter";}
};

class PurpleHazeFilter : public Filter
{
public:
    PurpleHazeFilter(){};
    void apply(Image* img);
    virtual char* getName() {return "PurpleHazeFilter";}

private:
    Pixel convertToPurple(Pixel p);
};

class LineFilter : public Filter
{
public:
    LineFilter(){};
    void apply(Image* img);
    virtual char* getName() {return "LineFilter";}

private:
    void drawLine(int x1, int y1, int x2, int y2, Image* img);
};

#endif // FILTER_HPP
