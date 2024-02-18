#ifndef PIXEL_HPP
#define PIXEL_HPP

class Pixel
{
public:
    Pixel(unsigned char r, unsigned char g, unsigned char b);
    Pixel(int r, int g, int b);
    unsigned char blue;
    unsigned char green;
    unsigned char red;

private:
    unsigned char limitValue(int value);
};

#endif //PIXEL_HPP
