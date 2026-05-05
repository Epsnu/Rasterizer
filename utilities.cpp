#include <fstream>
#include <iostream>
#include <cmath>

#include "utilities.h"
#include "classes.h"
#include "string_utils.h"

/**
 * Parses input file by reading each line
 * Each line may fit one of the given keywords
 * and this extracts the values into them.
 * @param inputFile
 * @return
 */
std::pair<PNG_dim, std::queue<DrawCommand>> parseInputFile(std::string inputFile) {
    // open input file
    std::ifstream inFile(inputFile);
    if (!inFile.is_open()) {
        throw std::runtime_error("Error opening file\n");
    }

    // set png dimensions
    std::string line;
    std::getline(inFile, line);
    PNG_dim png_dim = evaluate_png_line(line);

    /* Go through the rest of the lines.
    When you see a new drawArrayTriangles (or ''Elements''),
    set the current states into a new ImageData
    and start checking lines again. */
    std::queue<DrawCommand> command_queue;
    // for each line, get data.
    // once you reach a new dAT line, add the command it specifies to the queue.
    std::vector<Position4> positions;
    std::vector<Color4> colors;
    std::vector<Texcoord> texcoords;
    std::vector<Pointsize> pointsizes;
    DrawArraysTriangles dAT;
    while (std::getline(inFile, line)) {
        trim(line);
        std::vector<std::string> line_vec = split_by_whitespace(line);
        if (line_vec.size() == 0){
        } else if (line_vec[0] == "position") {
            positions = evaluate_positions_line(line_vec);
        } else if (line_vec[0] == "color") {
            colors = evaluate_colors_line(line_vec);
        } else if (line_vec[0] == "pointsize") {
            pointsizes = evaluate_pointsize_line(line_vec);
        } else if (line_vec[0] == "texture") {
            throw std::runtime_error("Texturing not yet implemented\n");
        } else if (line_vec[0] == "drawArraysTriangles") {
            dAT = evaluate_drawArraysTriangles_line(line_vec);
            // create a DrawCommand with the current state when we find a call to draw some stuff
            command_queue.push(createCommand(positions, colors, texcoords, pointsizes, dAT, png_dim));
        }
    }

    return {png_dim, command_queue};
}

DrawCommand createCommand(std::vector<Position4>& pos,
                                                std::vector<Color4>& col,
                                                std::vector<Texcoord>& tex,
                                                std::vector<Pointsize>& psz,
                                                DrawArraysTriangles& dAT,
                                                PNG_dim& png_dim)
{
    // if tex and psz were not part of the file read
    if (tex.size() == 0) { for (size_t i=0; i<col.size(); i++) tex.push_back(Texcoord()); }
    if (psz.size() == 0) { for (size_t i=0; i<col.size(); i++) psz.push_back(Pointsize()); }

    // place three vertices into a triangle, then add that triangle into the command queue
    // command queue will only be as long as dAT.count_ / 3 (number of triangles to draw),
    // despite there being more colors, points, etc. to choose from.
    DrawCommand command(png_dim);
    Triangle t;
    for (unsigned int i = dAT.first_; i < (dAT.count_ + dAT.first_); i++) {
        if ((i-dAT.first_)%3 == 0) {
            t.v1_ = Vertex(pos.at(i), col.at(i), tex.at(i), psz.at(i));
        } else if ((i-dAT.first_)%3 == 1) {
            t.v2_ = Vertex(pos.at(i), col.at(i), tex.at(i), psz.at(i));
        } else {
            t.v3_ = Vertex(pos.at(i), col.at(i), tex.at(i), psz.at(i));
            command.addTriangle(t);
        }
    }
    return command;
}

PNG_dim evaluate_png_line(std::string& line) {
    trim(line);
    std::vector<std::string> line_vec = split_by_whitespace(line);
    if (line_vec.size() < 4) {
        throw std::invalid_argument("Invalid png line\n");
    }
    unsigned int width = std::stoi(line_vec[1]);
    unsigned int height = std::stoi(line_vec[2]);
    std::string outputFile = line_vec[3];
    return PNG_dim(width, height, outputFile);
}

std::vector<Position4> evaluate_positions_line(const std::vector<std::string>& line_vec) {
    unsigned int size = std::stoi(line_vec[1]);

    std::vector<float> positions_raw;
    for (size_t i=2; i<line_vec.size(); i++) {
        positions_raw.push_back(std::stof(line_vec[i]));
    }

    if (int(positions_raw.size())%size != 0) {
        throw std::invalid_argument("Invalid vec to parse positions with given size\n");
    }

    std::vector<Position4> positions;
    Position4 pos4;
    for (size_t i=0; i < positions_raw.size(); i++) {
        // set each x,y,z,w value
        switch (i%size)
        {
        case 0:
            pos4.x_ = positions_raw[i];
            break;
        case 1:
            pos4.y_ = positions_raw[i];
            break;
        case 2:
            pos4.z_ = positions_raw[i];
            break;
        case 3:
            pos4.w_ = positions_raw[i];
            break;
        }
        // if x,y,z,w (or other permutations) are set, input into pos vec
        if (i%size == (size-1)) {
            positions.push_back(pos4);
        }
    }

    return positions;
}

std::vector<Color4> evaluate_colors_line(const std::vector<std::string>& line_vec) {
    unsigned int size = std::stoi(line_vec[1]);

    std::vector<float> colors_raw;
    for (size_t i=2; i < line_vec.size(); i++) {
        colors_raw.push_back(std::stof(line_vec[i]));
    }

    if (int(colors_raw.size())%size != 0) {
        throw std::invalid_argument("Invalid vec to parse colors with given size\n");
    }

    std::vector<Color4> colors;
    Color4 col4;
    for (size_t i=0; i < colors_raw.size(); i++) {
        const auto& color = colors_raw[i];
        // throw error if colors out of bounds
        if (color > 1 || color < 0) {
            throw std::invalid_argument("Color intensity out of bounds\n");
        }
        // set each r,g,b,a value
        switch (i%size)
        {
        case 0:
            col4.r_ = color;
            break;
        case 1:
            col4.g_ = color;
            break;
        case 2:
            col4.b_ = color;
            break;
        case 3:
            col4.a_ = color;
            break;
        }
        // if r,g,b,a (or other permutations) are set, input into color vec
        if (i%size == (size-1)) {
            if (size == 3) { col4.a_ = 1.0; } // if no alpha given, set to 1.0
            colors.push_back(col4);
        }
    }

    return colors;
}

std::vector<Pointsize> evaluate_pointsize_line(const std::vector<std::string>& line_vec) {
    Pointsize tmp;
    std::vector<Pointsize> pszs;
    for (size_t i=2; i<line_vec.size(); i++) {
        tmp.p_ = std::stof(line_vec[i]);
        pszs.push_back(tmp);
    }
    return pszs;
}

DrawArraysTriangles evaluate_drawArraysTriangles_line(const std::vector<std::string>& line_vec) {
    if (line_vec.size() != 3) {
        throw std::invalid_argument("Invalid drawArraysTriangles line\n");
    }
    DrawArraysTriangles dAT;
    dAT.first_ = std::stoi(line_vec[1]);
    dAT.count_ = std::stoi(line_vec[2]);
    return dAT;
}

/**
 * Converts from linear color space to sRGB color space
 * Formula from https://cs418.cs.illinois.edu/website/text/color.html
 */
float LinearToSRGBHelper(float c) {
    if (c <= 0.0031308) {
        return 12.92 * c;
    } else {
        return 1.055 * std::pow(c, 1.0/2.4) - 0.055;
    }
}

std::tuple<int, int, int, int> LinearToSRGB(std::tuple<float, float, float, float> color){
    float r = std::get<0>(color);
    float g = std::get<1>(color);
    float b = std::get<2>(color);
    float a = std::get<3>(color);

    // apply linear to sRGB conversion
    r = LinearToSRGBHelper(r);
    g = LinearToSRGBHelper(g);
    b = LinearToSRGBHelper(b);

    return {
        std::round(r * 255.0),
        std::round(g * 255.0),
        std::round(b * 255.0),
        std::round(a * 255.0)
    };
}