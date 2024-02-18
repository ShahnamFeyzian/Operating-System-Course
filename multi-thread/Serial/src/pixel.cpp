#include "pixel.hpp"

Pixel::Pixel(unsigned char r, unsigned char g, unsigned char b) 
            : red(r), green(g), blue(b){}

Pixel::Pixel(int r, int g, int b)
{
    red   = limitValue(r);
    green = limitValue(g);
    blue  = limitValue(b);
}

unsigned char Pixel::limitValue(int value)
{
    if(value > 255) return 255;
    else if(value < 0) return 0;
    else return value;
}
