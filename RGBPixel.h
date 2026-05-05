#pragma once

#include <iostream>
#include <sstream>

class RGBPixel {
  public:
    unsigned int r;
    unsigned int g;
    unsigned int b;
    double a = 0;

    RGBPixel();

    RGBPixel(unsigned int r_, unsigned int g_, unsigned int b_);

    RGBPixel(unsigned int r_, unsigned int g_, unsigned int b_, double a_);

    RGBPixel diff(RGBPixel const & other);

    bool operator== (RGBPixel const & other) const;
    bool operator!= (RGBPixel const & other) const;
    bool operator<  (RGBPixel const & other) const;
};

unsigned int AbsDiff(unsigned int a, unsigned int b);

/**
 * Stream operator that allows pixels to be written to standard streams
 * (like cout).
 *
 * @param out Stream to write to.
 * @param pixel Pixel to write to the stream.
 */
std::ostream & operator<<(std::ostream & out, RGBPixel const & pixel);
std::stringstream & operator<<(std::stringstream & out, RGBPixel const & pixel);