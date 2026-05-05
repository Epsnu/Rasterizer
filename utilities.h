#ifndef UTILITIES_H
#define UTILITIES_H

#include "classes.h"
#include "PNG.h"

#include <cmath>


std::pair<PNG_dim, std::queue<DrawCommand>> parseInputFile(std::string inputFile);

DrawCommand createCommand(std::vector<Position4>& positions,
                                                std::vector<Color4>& colors,
                                                std::vector<Texcoord>& texcoords,
                                                std::vector<Pointsize>& pointsizes,
                                                DrawArraysTriangles& dAT,
                                                PNG_dim& png_dim
);

PNG_dim evaluate_png_line(std::string& line);
std::vector<Position4> evaluate_positions_line(const std::vector<std::string>& line_vec);
std::vector<Color4> evaluate_colors_line(const std::vector<std::string>& line_vec);
std::vector<Pointsize> evaluate_pointsize_line(const std::vector<std::string>& line_vec);
DrawArraysTriangles evaluate_drawArraysTriangles_line(const std::vector<std::string>& line_vec);

template<typename T>
std::vector<T> operator+(const std::vector<T>& a, const std::vector<T>& b) {
    if (a.size() != b.size()) {
        throw std::invalid_argument("Cannot perform \"+\" operation on different sized vectors");
    }
    std::vector<T> sum;
    for (size_t i=0; i<a.size(); i++) {
        sum.push_back(a[i] + b[i]);
    }
    return sum;
};

template<typename T>
std::vector<T> operator-(const std::vector<T>& a, const std::vector<T>& b) {
    if (a.size() != b.size()) {
        throw std::invalid_argument("Cannot perform \"-\" operation on different sized vectors");
    }
    std::vector<T> diff;
    for (size_t i=0; i<a.size(); i++) {
        diff.push_back(a[i] - b[i]);
    }
    return diff;
};

template<typename T>
std::vector<T> operator*(const std::vector<T>& a, const float& b) {
    std::vector<T> quotient;
    for (size_t i=0; i<a.size(); i++) {
        quotient.push_back(a[i] * b);
    }
    return quotient;
};

float LinearToSRGBHelper(float c);

std::tuple<int, int, int, int> LinearToSRGB(std::tuple<float, float, float, float> color);

#endif