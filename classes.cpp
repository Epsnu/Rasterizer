#include <cmath>
#include <algorithm>

#include "classes.h"
#include "rasterize.h"
#include "utilities.h"

DrawCommand::DrawCommand(PNG_dim& png_dim) {
    z_buffer_ = std::vector<float>(png_dim.width_ * png_dim.height_, 1.0f);
}

void DrawCommand::drawTriangle(PNG* png) {
    // draw a triangle using the scanline algorithm
    Vertex p = triangles_.front().v1_;
    Vertex q = triangles_.front().v2_;
    Vertex r = triangles_.front().v3_;
    triangles_.pop();

    // divide by w
    DivideByW(p,q,r);

    // viewport transform to image coords
    ViewportTransform(png, p, q, r);

    // call scanline to get all pixels in triangle
    bool hypPerspCorr = true; // (always enabled in WebGL2)
    std::vector<Vertex> pixels = scanline(p, q, r, hypPerspCorr);

    // Recover actual attribute data by dividing each attribute by 1/w
    DivideBy1OverW(pixels);

    // color in all pixels on png
    DrawOnPng(png, pixels);
}

/**
 * Divide by w except for w = 1/w
 */
void DrawCommand::DivideByW(Vertex& p, Vertex& q, Vertex& r) {
    float w_p = p.at(3);
    float w_q = q.at(3);
    float w_r = r.at(3);
    if (w_p == 0 || w_q == 0 || w_r == 0) { throw std::invalid_argument("w coordinate of vertex cannot be 0"); }
    // divide all positions and attributes by w
    for (size_t i=0; i<4; i++) {
        p.at(i) /= w_p;
        q.at(i) /= w_q;
        r.at(i) /= w_r;
    }
    for (size_t i=4; i<p.size(); i++) {
        p.at(i) /= w_p;
        q.at(i) /= w_q;
        r.at(i) /= w_r;
    }
    // set 1/w to w
    p.at(3) = 1.0/w_p;
    q.at(3) = 1.0/w_q;
    r.at(3) = 1.0/w_r;
}

void DrawCommand::DivideBy1OverW(std::vector<Vertex>& vertices) {
    for (auto& v : vertices) {
        float inv_w = v.at(3);
        if (inv_w == 0) { throw std::invalid_argument("w coordinate of vertex cannot be 0"); }
        // divide all attributes by 1/w
        for (size_t i=4; i<v.size(); i++) {
            v.at(i) /= inv_w;
        }
        // set 1/w to w
        v.at(3) = 1.0/inv_w;
    }
}

/**
 * Transform x and y coords from NDC to viewport coords
 */
void DrawCommand::ViewportTransform(PNG* png, Vertex& p, Vertex& q, Vertex& r) {
    // Transform coordinates from NDC to viewport
    p.at(0) = (p.at(0)+1.0)*(png->width()/2.0);
    p.at(1) = (1.0-p.at(1))*(png->height()/2.0);

    q.at(0) = (q.at(0)+1.0)*(png->width()/2.0);
    q.at(1) = (1.0-q.at(1))*(png->height()/2.0);

    r.at(0) = (r.at(0)+1.0)*(png->width()/2.0);
    r.at(1) = (1.0-r.at(1))*(png->height()/2.0);

    // quirk of my PNG library: need to flip y axis
    p.at(1) = png->height() - p.at(1);
    q.at(1) = png->height() - q.at(1);
    r.at(1) = png->height() - r.at(1);
}

/**
 * Draw all pixels on png
 */
void DrawCommand::DrawOnPng(PNG* png, const std::vector<Vertex>& pixels) {
    for (size_t i=0; i<pixels.size(); i++) {
        const auto& pixel = pixels.at(i);

        float x = pixel.at(0); // x coord
        float y = pixel.at(1); // y coord

        auto [r, g, b, a] = LinearToSRGB({pixel.at(4), pixel.at(5), pixel.at(6), pixel.at(7)});
        
        // error checks
        if (x < 0 || x >= png->width() || y < 0 || y >= png->height()) {
            // throw std::out_of_range("What the heck part 1");
            std::cout << "Warning: pixel (" << x << "," << y << ") is out of bounds" << std::endl;
            x = std::min((int)png->width()-1, std::max(0, (int)std::round(x)));
            y = std::min((int)png->height()-1, std::max(0, (int)std::round(y)));
        }
        if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255 || a < 0 || a > 255) {
            // throw std::out_of_range("What the heck part 2");
            // clamp colors to [0,255]
            r = std::min(255, std::max(0, r));
            g = std::min(255, std::max(0, g));
            b = std::min(255, std::max(0, b));
            a = std::min(255, std::max(0, a));
        }

        // check if pixel is above current pixel in z-buffer
        float z = pixel.at(2); // z coord
        z = std::min(1.0f, std::max(0.0f, z)); // clamp z to [0,1]
        z_buffer_.at(i) = z; // update z-buffer

        size_t idx = (size_t)(y*png->width() + x);
        if (z < z_buffer_.at(idx)) {
            z_buffer_.at(idx) = z; // update z-buffer
            // get png pixel by reference and set color and other specifications
            png->getPixel(x, y) = RGBPixel(r,g,b,a);
        }
    }
}