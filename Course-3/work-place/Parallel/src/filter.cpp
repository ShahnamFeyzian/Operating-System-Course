#include "filter.hpp"

#include "pthread.h"

#include<vector>

#include "image.hpp"
#include "pixel.hpp"
#include "thread.hpp"

void Filter::initialize(Image* img)
{
    img_ = img;
    originalPixels_ = img->getPixels();
}

void ParallelBlurFilter::apply(Image* img)
{
    initialize(img);
    int partitionSize = img_->getRows() / thrd::NUM_THREADS;

    std::vector<ApplyUnitArguments*> args;
    for(int i=0; i<thrd::NUM_THREADS-1; i++)
        args.push_back(new ApplyUnitArguments(this, i*partitionSize, (i+1)*partitionSize));
    args.push_back(new ApplyUnitArguments(this, (thrd::NUM_THREADS-1)*partitionSize, img_->getRows()));

    pthread_t threads[thrd::NUM_THREADS];
    for(int i=0; i<thrd::NUM_THREADS; i++)
        pthread_create(&threads[i], NULL, ParallelBlurFilter::applyUnitT, args[i]);
    
    for(int i=0; i<thrd::NUM_THREADS; i++)
        pthread_join(threads[i], NULL);
        
}

void *ParallelBlurFilter::applyUnitT(void *args)
{
    ApplyUnitArguments* params = (ApplyUnitArguments*)args;
    params->currentClass->applyUnit(params->start, params->end);
    return nullptr;
}

void ParallelBlurFilter::applyUnit(int start, int end)
{
    for(int r=start; r<end; r++)
        for(int c=0; c<img_->getCols(); c++)
            (*img_)(r, c) = calcNewPixel(r, c);
}

Pixel ParallelBlurFilter::calcNewPixel(int row, int col)
{
    std::vector<std::vector<Pixel>> pixelMatrix = generatePixelMatrix(row, col);
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

std::vector<std::vector<Pixel>> ParallelBlurFilter::generatePixelMatrix(int row, int col)
{
    std::vector<std::vector<Pixel>> matrix;
    int maxRow = originalPixels_.size() - 1;
    int maxCol = originalPixels_[0].size() - 1;

    for(int r=0; r<3; r++)
    {
        matrix.push_back(std::vector<Pixel>());
        for(int c=0; c<3; c++)
        {
            matrix[r].push_back((isOutOfSize(maxCol, maxRow, row+r-1, col+c-1)) ? originalPixels_[row][col] : originalPixels_[row+r-1][col+c-1]);
        }
    }
    return matrix;
}

bool ParallelBlurFilter::isOutOfSize(int maxCol, int maxRow, int row, int col)
{
    return (row > maxRow) || (col > maxCol) || (col < 0) || (row < 0);
}

void ParallelFlipFilter::apply(Image* img)
{
    initialize(img);
    int partitionSize = img_->getRows() / thrd::NUM_THREADS;

    std::vector<ApplyUnitArguments*> args;
    for(int i=0; i<thrd::NUM_THREADS-1; i++)
        args.push_back(new ApplyUnitArguments(this, i*partitionSize, (i+1)*partitionSize));
    args.push_back(new ApplyUnitArguments(this, (thrd::NUM_THREADS-1)*partitionSize, img_->getRows()));

    pthread_t threads[thrd::NUM_THREADS];
    for(int i=0; i<thrd::NUM_THREADS; i++)
        pthread_create(&threads[i], NULL, ParallelFlipFilter::applyUnitT, args[i]);
    
    for(int i=0; i<thrd::NUM_THREADS; i++)
        pthread_join(threads[i], NULL);
}

void *ParallelFlipFilter::applyUnitT(void *args)
{
    ApplyUnitArguments* params = (ApplyUnitArguments*)args;
    params->currentClass->applyUnit(params->start, params->end);
    return nullptr;
}

void ParallelFlipFilter::applyUnit(int start, int end)
{
    for(int r=start; r<end; r++)
        for(int c=0; c<img_->getCols(); c++)
            (*img_)(r, c) = originalPixels_[img_->getRows() - r - 1][c];
}

void ParallelPurpleHazeFilter::apply(Image* img)
{
    initialize(img);
    int partitionSize = img_->getRows() / thrd::NUM_THREADS;

    std::vector<ApplyUnitArguments*> args;
    for(int i=0; i<thrd::NUM_THREADS-1; i++)
        args.push_back(new ApplyUnitArguments(this, i*partitionSize, (i+1)*partitionSize));
    args.push_back(new ApplyUnitArguments(this, (thrd::NUM_THREADS-1)*partitionSize, img_->getRows()));

    pthread_t threads[thrd::NUM_THREADS];
    for(int i=0; i<thrd::NUM_THREADS; i++)
        pthread_create(&threads[i], NULL, ParallelPurpleHazeFilter::applyUnitT, args[i]);
    
    for(int i=0; i<thrd::NUM_THREADS; i++)
        pthread_join(threads[i], NULL);
}

void *ParallelPurpleHazeFilter::applyUnitT(void *args)
{
    ApplyUnitArguments* params = (ApplyUnitArguments*)args;
    params->currentClass->applyUnit(params->start, params->end);
    return nullptr;
}

void ParallelPurpleHazeFilter::applyUnit(int start, int end)
{
    for(int r=start; r<end; r++)
        for(int c=0; c<img_->getCols(); c++)
                (*img_)(r, c) = convertToPurple((*img_)(r, c));
}

Pixel ParallelPurpleHazeFilter::convertToPurple(Pixel p)
{
    int r = p.red*0.5  + p.green*0.3 + p.blue*0.5;
    int g = p.red*0.16 + p.green*0.5 + p.blue*0.16;
    int b = p.red*0.6  + p.green*0.2 + p.blue*0.8;
    return Pixel(r, g, b);
}

void ParallelLineFilter::apply(Image* img)
{
    initialize(img);
    int maxCol = img->getCols() - 1;
    int maxRow = img->getRows() - 1;
    
    pthread_t threads[3];
    std::vector<DrawLineArguments*> args;
    args.push_back(new DrawLineArguments(this, maxCol, 0, 0, maxRow));
    args.push_back(new DrawLineArguments(this, maxCol/2, 0, 0, maxRow/2));
    args.push_back(new DrawLineArguments(this, maxCol/2, maxRow, maxCol, maxRow/2));
    
    for(int i=0; i<3; i++)
        pthread_create(&threads[i], NULL, ParallelLineFilter::drawLineT, args[i]);
    
    for(int i=0; i<3; i++)
        pthread_join(threads[i], NULL);

}

void *ParallelLineFilter::drawLineT(void *args)
{
    DrawLineArguments* params = (DrawLineArguments*)args;
    params->currentClass->drawLine(params->x1, params->y1, params->x2, params->y2);
    return nullptr;
}

void ParallelLineFilter::drawLine(int x1, int y1, int x2, int y2)
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
        (*img_)(i*a+minY, i+minX) = Pixel(255, 255, 255);
    }
}
