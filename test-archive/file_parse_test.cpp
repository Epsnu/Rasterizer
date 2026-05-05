#include "utilities.h"


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Not enough input arguments.";
        return 1;
    }
    std::string inputFile = argv[1];

    // Not sure what return type to use!
    ImageData* image_data = parseInputFile(inputFile);

    delete image_data;

    return 0;
}