#ifndef CLASSES_H
#define CLASSES_H

#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>
#include <queue>
#include <array>

#include "PNG.h"

const unsigned int vertex_size = 11;


struct Position4 {
    float x_,y_,z_,w_ = 0.0;
};

struct Color4 {
    float r_,g_,b_ = 0.0;
    float a_ = 1.0;
};

struct Texcoord {
    float s_, t_ = 0.0;
};

struct Pointsize {
    float p_ = 1.0;
};

struct DrawArraysTriangles {
    unsigned int first_, count_;
};

class Vertex {
    public:
    Vertex() { state_.fill(0.0f); };

    Vertex(const Position4& pos, const Color4& col,
    const Texcoord& tex = Texcoord(), const Pointsize& psz = Pointsize()
    ) {
        state_ = {
            pos.x_, pos.y_, pos.z_, pos.w_,
            col.r_, col.g_, col.b_, col.a_,
            tex.s_, tex.t_, psz.p_
        };
        empty_ = false;
    };

    Vertex(std::array<float, vertex_size> s) : state_(s), empty_(false) {};
    
    float at(const unsigned int& idx) const {
        if (idx >= state_.size()) {
            throw std::out_of_range("Index out of range in Vertex::at");
        }
        return state_[idx];
    }
    float& at(const unsigned int& idx) {
        if (idx >= state_.size()) {
            throw std::out_of_range("Index out of range in Vertex::at");
        }
        return state_[idx];
    }
    
    unsigned int size() const { return vertex_size; };

    Vertex operator+(const Vertex& a) {
        std::array<float, vertex_size> new_state;
        for (size_t i=0; i<state_.size(); i++) {
            new_state[i] = state_[i] + a.state_[i];
        }
        if (!a.empty_) { empty_ = false; }
        return Vertex(new_state);
    }
    Vertex operator-(const Vertex& a) {
        std::array<float, 11> new_state;
        for (size_t i=0; i<state_.size(); i++) {
            new_state[i] = state_[i] - a.state_[i];
        }
        if (!a.empty_) { empty_ = false; }
        return Vertex(new_state);
    }
    Vertex operator*(const float& a) {
        std::array<float, vertex_size> new_state;
        for (size_t i=0; i<state_.size(); i++) {
            new_state[i] = state_[i]*a;
        }
        return Vertex(new_state);
    }
    Vertex operator/(const float& a) {
        return operator*(1.0/a);
    }

    Vertex& operator=(const Vertex& a) {
        state_ = a.state_;
        empty_ = a.empty_;
        return *this;
    }
    
    // follows a standard of (x,y,z,w,r,g,b,a,s,t,p)
    std::array<float, vertex_size> state_;
    bool empty_ = true;
};

struct Triangle {
    Vertex v1_;
    Vertex v2_;
    Vertex v3_;
};

class PNG_dim {
    public:
        PNG_dim() {};

        PNG_dim(unsigned int width, unsigned int height, std::string filename)
            : width_(width), height_(height), filename_(filename) {};

        unsigned int width_ = 0;
        unsigned int height_ = 0;
        std::string filename_;
};

class DrawCommand {
    public:
    DrawCommand(PNG_dim& png_dim);
    void addTriangle(Triangle& t) { triangles_.push(t); };
    void drawTriangle(PNG* png); // unimplemented
    void drawAll(PNG* png) { while (!triangles_.empty()) { drawTriangle(png); } };
    
    std::queue<Triangle> triangles_;
    std::vector<float> z_buffer_;

    private:
    void DivideByW(Vertex& p, Vertex& q, Vertex& r);
    void DivideBy1OverW(std::vector<Vertex>& vertices);
    void ViewportTransform(PNG* png, Vertex& p, Vertex& q, Vertex& r);
    void DrawOnPng(PNG* png, const std::vector<Vertex>& pixels);
};

#endif