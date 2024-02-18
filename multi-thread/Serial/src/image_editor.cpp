#include "image_editor.hpp"


#include<chrono>
#include<iostream>
#include<string>

#include "filter.hpp"
#include "image.hpp"

using namespace std::chrono;

ImageEditor::ImageEditor()
{
    filterPipe_.push_back(new FlipFilter());
    filterPipe_.push_back(new BlurFilter());
    filterPipe_.push_back(new PurpleHazeFilter());
    filterPipe_.push_back(new LineFilter());
}

void ImageEditor::editImage(const std::string& inputPath, const std::string& outputPath)
{
    Image* img = new Image();
    
    auto start = high_resolution_clock::now();
    img->readBmp(inputPath);
    auto stop = high_resolution_clock::now();
    std::cout << "Reading image duration: " << duration_cast<microseconds>(stop - start).count() << std::endl;

    for(int i=0; i<filterPipe_.size(); i++)
    {
        start = high_resolution_clock::now();
        filterPipe_[i]->apply(img);
        stop = high_resolution_clock::now();
        std::cout << "Applying " << filterPipe_[i]->getName() << " duration: " << duration_cast<microseconds>(stop - start).count() << std::endl;
    }
    
    start = high_resolution_clock::now();   
    img->writeBmp(outputPath);
    stop = high_resolution_clock::now();
    std::cout << "Writing image duration: " << duration_cast<microseconds>(stop - start).count() << std::endl;
}
