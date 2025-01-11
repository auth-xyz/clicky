#include "clicky/clicky.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    clicky parser;

    parser.add_argument("input", "i", "Path to the input file", true);
    parser.add_argument("output", "o", "Path to the output file", true);

    parser.add_option("verbose", "v", "Enable verbose output", false);
    parser.add_option("overwrite", "c", "Overwrite existing files", false);

    parser.parse(argc, argv);

    std::string input = parser.argument("input");
    std::string output = parser.argument("output");
    bool verbose = parser.option("verbose");
    bool overwrite = parser.option("overwrite");

    if (verbose) {
        std::cout << "Input file: " << input << "\n";
        std::cout << "Output file: " << output << "\n";
        std::cout << "Overwrite: " << (overwrite ? "Yes" : "No") << "\n";
    }

    return 0;
}

// Usage:
// ./program --input path/to/file --output path/to/file --verbose --overwrite

