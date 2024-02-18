#include "bmp.hpp"

#include<fstream>
#include<string>
#include<vector>

#include<pthread.h>

#include "pixel.hpp"
#include "thread.hpp"

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
    std::vector<std::vector<Pixel>> pixels(rows_, std::vector<Pixel>(cols_, Pixel(0,0,0)));
    int partitionSize = rows_ / thrd::NUM_THREADS;

    std::vector<GeneratePixelArgument*> args;
    for(int i=0; i<thrd::NUM_THREADS-1; i++)
        args.push_back(new GeneratePixelArgument(this, &pixels, i*partitionSize, (i+1)*partitionSize));
    args.push_back(new GeneratePixelArgument(this, &pixels, (thrd::NUM_THREADS-1)*partitionSize, rows_));

    pthread_t threads[thrd::NUM_THREADS];
    for(int i=0; i<thrd::NUM_THREADS; i++)
        pthread_create(&threads[i], NULL, generatePixelUnitT, args[i]);
    
    for(int i=0; i<thrd::NUM_THREADS; i++)
        pthread_join(threads[i], NULL);

    return pixels;
}

void *Bmp::generatePixelUnitT(void *args)
{
    GeneratePixelArgument* params = (GeneratePixelArgument*)args;
    params->currentClass->generatePixelUnit((*params->pixels), params->start, params->end);
    return nullptr;
}

void Bmp::generatePixelUnit(std::vector<std::vector<Pixel>>& pixels, int start, int end)
{
    for(int r=start; r<end; r++)
        for(int c=0; c<cols_; c++)
            pixels[r][c] = getPixel(r, c);   
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

void Bmp::write(const std::string& bmpPath, std::vector<std::vector<Pixel>>& matrix)
{
    int partitionSize = rows_ / thrd::NUM_THREADS;

    std::vector<GeneratePixelArgument*> args;
    for(int i=0; i<thrd::NUM_THREADS-1; i++)
        args.push_back(new GeneratePixelArgument(this, &matrix, i*partitionSize, (i+1)*partitionSize));
    args.push_back(new GeneratePixelArgument(this, &matrix, (thrd::NUM_THREADS-1)*partitionSize, rows_));

    pthread_t threads[thrd::NUM_THREADS];
    for(int i=0; i<thrd::NUM_THREADS; i++)
        pthread_create(&threads[i], NULL, writeUnitT, args[i]);
    
    for(int i=0; i<thrd::NUM_THREADS; i++)
        pthread_join(threads[i], NULL);

    std::ofstream file(bmpPath, std::ios_base::binary);
    file.write(fileData_, fileHeader_->bfSize);
}

void *Bmp::writeUnitT(void *args)
{
    GeneratePixelArgument* params = (GeneratePixelArgument*)args;
    params->currentClass->writeUnit((*params->pixels), params->start, params->end);
    return nullptr;
}

void Bmp::writeUnit(const std::vector<std::vector<Pixel>>& matrix, int start, int end)
{
    for (int r = start; r < end; ++r) 
        for (int c = 0; c < cols_; ++c) 
            getPixel(r, c) = matrix[r][c];
}

int Bmp::getRows()
{
    return rows_;
}

int Bmp::getCols()
{
    return cols_;
}
