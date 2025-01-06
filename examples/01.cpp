#include "clicky/clicky.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    clicky parser;

    // Adding positional arguments
    parser.add_argument({"input", "entrance"}, {"i"}, true, "Path to the input file");
    parser.add_argument({"output"}, {"o", "e"}, true, "Path to the output file");

    // Adding options
    parser.add_option({"verbose"}, {"v"}, false, "Enable verbose output");
    parser.add_option({"overwrite"}, {"c"}, false, "Overwrite existing files");

    // Parsing command-line arguments
    parser.parse(argc, argv);

    // Accessing values
    std::string input = parser.argument("input");
    std::string output = parser.argument("output");
    bool verbose = parser.option("verbose");
    bool overwrite = parser.option("overwrite");

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

