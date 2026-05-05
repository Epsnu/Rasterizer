#include "RGBPixel.h"
#include <cmath>
#include <iostream>

RGBPixel::RGBPixel() {
    r = 0;
    g = 0;
    b = 0;
    a = 1.0;
}

RGBPixel::RGBPixel(unsigned int r_, unsigned int g_, unsigned int b_) {
    r = r_;
    g = g_;
    b = b_;
    a = 1.0;
}

RGBPixel::RGBPixel(unsigned int r_, unsigned int g_, unsigned int b_, double a_) {
    r = r_;
    g = g_;
    b = b_;
    a = a_;
}

RGBPixel RGBPixel::diff(RGBPixel const & other) {
    if (other == *this) return RGBPixel();

    RGBPixel diff(
        AbsDiff(r, other.r),
        AbsDiff(r, other.g),
        AbsDiff(r, other.b),
        AbsDiff(r, other.a)
    );

    return diff;
}

bool RGBPixel::operator== (RGBPixel const & other) const {
    if (fabs(a - other.a) > 0.0000001) { return false; }
    if ( a == 0 ) { return true; }

    if (r==other.r && g==other.g && b==other.b) { return true; }
    return false;
}

bool RGBPixel::operator!= (RGBPixel const & other) const {
    return !(*this == other);
}

bool RGBPixel::operator<  (RGBPixel const & other) const {
    if (r < other.r)
        return true;
    else if (r > other.r)
        return false;
    // == hue

    if (g < other.g)
        return true;
    else if (g > other.g)
        return false;
    // == saturation

    if (b < other.b)
        return true;
    else if (b > other.b)
        return false;
    // == lumininance

    if (a < other.a)
        return true;
    else if (a > other.a)
        return false;
    // == alpha

    // same pixel
    return false;
}

std::ostream & operator<<(std::ostream & out, RGBPixel const & pixel) {
    out << "(" << pixel.r << ", " << pixel.g << ", " << pixel.b << (pixel.a != 1 ? ", " + std::to_string(pixel.a) : "") << ")";

    return out;
}

unsigned int AbsDiff(unsigned int a, unsigned int b) {
  if (a>b) {
    return a-b;
  }
  return b-a;
};