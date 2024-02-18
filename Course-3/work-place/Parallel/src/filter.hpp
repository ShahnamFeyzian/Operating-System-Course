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

protected:
    Image* img_;
    std::vector<std::vector<Pixel>> originalPixels_;

    void initialize(Image* img); 

};

class ParallelBlurFilter : public Filter
{
public:
    ParallelBlurFilter(){};
    void apply(Image* img);
    virtual char* getName() {return "BlureFilter";}

private:
    Pixel calcNewPixel(int row, int col);
    std::vector<std::vector<Pixel>> generatePixelMatrix(int row, int col);
    bool isOutOfSize(int maxCol, int maxRow, int row, int col);
    void applyUnit(int start, int end);
    static void *applyUnitT(void *args);

    struct ApplyUnitArguments
    {
        ApplyUnitArguments(ParallelBlurFilter* c, int s, int e)
                            : start(s), end(e) {currentClass = c;}

        ParallelBlurFilter* currentClass;
        int start, end;
    };
};

class ParallelFlipFilter : public Filter
{
public:
    ParallelFlipFilter(){};
    void apply(Image* img);
    virtual char* getName() {return "FlipFilter";}

private:
    void applyUnit(int start, int end);
    static void *applyUnitT(void *args);

    struct ApplyUnitArguments
    {
        ApplyUnitArguments(ParallelFlipFilter* c, int s, int e)
                            : start(s), end(e) {currentClass = c;}

        ParallelFlipFilter* currentClass;
        int start, end;
    };
};

class ParallelPurpleHazeFilter : public Filter
{
public:
    ParallelPurpleHazeFilter(){};
    void apply(Image* img);
    virtual char* getName() {return "PurpleHazeFilter";}

private:
    Pixel convertToPurple(Pixel p);
    void applyUnit(int start, int end);
    static void *applyUnitT(void *args);

    struct ApplyUnitArguments
    {
        ApplyUnitArguments(ParallelPurpleHazeFilter* c, int s, int e)
                            : start(s), end(e) {currentClass = c;}

        ParallelPurpleHazeFilter* currentClass;
        int start, end;
    };
};

class ParallelLineFilter : public Filter
{
public:
    ParallelLineFilter(){};
    void apply(Image* img);
    virtual char* getName() {return "LineFilter";}

private:
    static void *drawLineT(void *args);
    void drawLine(int x1, int y1, int x2, int y2);
    
    struct DrawLineArguments
    {
        DrawLineArguments(ParallelLineFilter* c, int x1, int y1, int x2, int y2) 
                            : x1(x1), y1(y1), x2(x2), y2(y2) {currentClass = c;}
        
        ParallelLineFilter* currentClass;
        int x1, y1, x2, y2;
    };
};

#endif // FILTER_HPP
