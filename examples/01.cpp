#include "clicky/clicky.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    clicky parser;

    // Adding positional arguments
    parser.add_argument("input", "", true, "Path to the input file");
    parser.add_argument("output", "", true, "Path to the output file");

    // Adding flags
    parser.add_flag("verbose", "v", false, "Enable verbose output");
    parser.add_flag("overwrite", "o", false, "Overwrite existing files");

    // Parsing command-line arguments
    parser.parse(argc, argv);

    // Accessing values
    std::string input = parser.argument("input");
    std::string output = parser.argument("output");
    bool verbose = parser.flag("verbose");
    bool overwrite = parser.flag("overwrite");

    // Example output
    if (verbose) {
        std::cout << "Input file: " << input << "\n";
        std::cout << "Output file: " << output << "\n";
        std::cout << "Overwrite: " << (overwrite ? "Yes" : "No") << "\n";
    }

    return 0;
}

// Usage:
// ./program --input path/to/file --output path/to/file --verbose --overwrite 

