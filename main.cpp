#include "utilities.h"


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Not enough input arguments.";
        return 1;
    }
    std::string inputFile = argv[1];

    // Not sure what return type to use!
    auto [png_dim, command_queue] = parseInputFile(inputFile);

    // initialize png (and get output file name)
    PNG* output_png = new PNG(png_dim.width_, png_dim.height_);
    std::string outputFile = png_dim.filename_;

    // draw each command from the queue.
    // Note that each command has several triangles to draw.
    // Use the func, drawAll to fulfill command.
    std::queue<DrawCommand> copy = command_queue;
    while (!copy.empty()) {
        DrawCommand command = copy.front();
        copy.pop();
        command.drawAll(output_png);
    }

    try {
        output_png->writeToFile(outputFile);
    } catch(const std::exception& e) {
        throw std::runtime_error("Failed to write PNG to " + outputFile + ".");
    }
    

    return 0;
}