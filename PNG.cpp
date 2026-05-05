#include <iostream>
using std::cerr;
using std::endl;

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <cassert>
#include <algorithm>
#include <functional>

#include "lodepng.h"
#include "PNG.h"

typedef struct {
  unsigned char r, g, b, a; // [0, 255]
} rgbaColor;

void PNG::_copy(PNG const & other) {
  // Clear self
  delete[] imageData_;

  // Copy `other` to self
  width_ = other.width_;
  height_ = other.height_;
  imageData_ = new RGBPixel[width_ * height_];
  for (unsigned i = 0; i < width_ * height_; i++) {
    imageData_[i] = other.imageData_[i];
  }
}

PNG::PNG() {
  width_ = 0;
  height_ = 0;
  imageData_ = NULL;
}

// PNG::PNG(unsigned int width, unsigned int height) {
//   width_ = width;
//   height_ = height;
//   imageData_ = new RGBPixel[width * height];
// }

PNG::PNG(PNG const & other) {
  imageData_ = NULL;
  _copy(other);
}

PNG::~PNG() {
  delete[] imageData_;
}

PNG const & PNG::operator=(PNG const & other) {
  if (this != &other) { _copy(other); }
  return *this;
}

bool PNG::operator== (PNG const & other) const {
  if (width_ != other.width_) { return false; }
  if (height_ != other.height_) { return false; }

  for (unsigned i = 0; i < width_ * height_; i++) {
    RGBPixel & p1 = imageData_[i];
    RGBPixel & p2 = other.imageData_[i];
    if (p1 != p2);
  }

  return true;
}

bool PNG::operator!= (PNG const & other) const {
  return !(*this == other);
}

PNG PNG::diff(PNG const & other) {
  if (other == *this) {
    return PNG(); // return empty PNG if same
  }
  if (other.width_ != width_ || other.height_ != height_) {
    throw std::invalid_argument("PNGs must have same dimensions to get difference");
  }

  PNG diff(width_, height_);
  unsigned int length = std::max(width_*height_, other.width_*other.height_);
  for (unsigned i = 0; i < length; i++) {
    diff.imageData_[i] = imageData_[i].diff(other.imageData_[i]);
  }

  return diff;
}

RGBPixel & PNG::_getPixelHelper(unsigned int x, unsigned int y) const {
  if (width_ == 0 || height_ == 0) {
    cerr << "ERROR: Call to PNG::getPixel() made on an image with no pixels." << endl;
    assert(width_ > 0);
    assert(height_ > 0);
  }

  if (x >= width_) {
    cerr << "WARNING: Call to PNG::getPixel(" << x << "," << y << ") tries to access x=" << x
        << ", which is outside of the image (image width: " << width_ << ")." << endl;
    cerr << "       : Truncating x to " << (width_ - 1) << endl;
    x = width_ - 1;
  }

  if (y >= height_) {
    cerr << "WARNING: Call to PNG::getPixel(" << x << "," << y << ") tries to access y=" << y
        << ", which is outside of the image (image height: " << height_ << ")." << endl;
    cerr << "       : Truncating y to " << (height_ - 1) << endl;
    y = height_ - 1;
  }

  unsigned index = x + (y * width_);
  return imageData_[index];
}

RGBPixel & PNG::getPixel(unsigned int x, unsigned int y) { return _getPixelHelper(x,y); }

const RGBPixel & PNG::getPixel(unsigned int x, unsigned int y) const { return _getPixelHelper(x,y); }

bool PNG::readFromFile(string const & fileName) {
  vector<unsigned char> byteData;
  unsigned error = lodepng::decode(byteData, width_, height_, fileName);

  if (error) {
    cerr << "PNG decoder error " << error << ": " << lodepng_error_text(error) << endl;
    return false;
  }

  delete[] imageData_;
  imageData_ = new RGBPixel[width_ * height_];

  for (unsigned i = 0; i < byteData.size(); i += 4) {
    rgbaColor rgb;
    rgb.r = byteData[i];
    rgb.g = byteData[i + 1];
    rgb.b = byteData[i + 2];
    rgb.a = byteData[i + 3];

    RGBPixel & pixel = imageData_[i/4];
    pixel.r = rgb.r;
    pixel.g = rgb.g;
    pixel.b = rgb.b;
    pixel.a = rgb.a;
  }

  return true;
}

bool PNG::writeToFile(string const & fileName) {
  unsigned char *byteData = new unsigned char[width_ * height_ * 4];

  for (unsigned i = 0; i < width_ * height_; i++) {
    rgbaColor rgb;
    rgb.r = imageData_[i].r;
    rgb.g = imageData_[i].g;
    rgb.b = imageData_[i].b;
    rgb.a = imageData_[i].a;

    byteData[(i * 4)]     = rgb.r;
    byteData[(i * 4) + 1] = rgb.g;
    byteData[(i * 4) + 2] = rgb.b;
    byteData[(i * 4) + 3] = rgb.a;
  }

  unsigned error = lodepng::encode(fileName, byteData, width_, height_);
  if (error) {
    cerr << "PNG encoding error " << error << ": " << lodepng_error_text(error) << endl;
  }

  delete[] byteData;
  return (error == 0);
}

unsigned int PNG::width() const {
  return width_;
}

unsigned int PNG::height() const {
  return height_;
}

void PNG::resize(unsigned int newWidth, unsigned int newHeight) {
  // Create a new vector to store the image data for the new (resized) image
  RGBPixel * newImageData = new RGBPixel[newWidth * newHeight];

  // Copy the current data to the new image data, using the existing pixel
  // for coordinates within the bounds of the old image size
  for (unsigned x = 0; x < newWidth; x++) {
    for (unsigned y = 0; y < newHeight; y++) {
      if (x < width_ && y < height_) {
        RGBPixel & oldPixel = this->getPixel(x, y);
        RGBPixel & newPixel = newImageData[ (x + (y * newWidth)) ];
        newPixel = oldPixel;
      }
    }
  }

  // Clear the existing image
  delete[] imageData_;

  // Update the image to reflect the new image size and data
  width_ = newWidth;
  height_ = newHeight;
  imageData_ = newImageData;
}

std::ostream & operator << ( std::ostream& os, PNG const& png ) {
  std::hash<double> hashFunction;
  std::size_t hash = 0;

  for (unsigned x = 0; x < png.width(); x++) {
    for (unsigned y = 0; y < png.height(); y++) {
      const RGBPixel &pixel = png.getPixel(x, y);
      hash ^= hashFunction(pixel.r);
      hash ^= hashFunction(pixel.g);
      hash ^= hashFunction(pixel.b);
      hash ^= hashFunction(pixel.a);
    }
  }

  os << "PNG(w=" << png.width() << ", h=" << png.height() << ", hash=" << std::hex << hash << std::dec << ")";
  return os;
}
