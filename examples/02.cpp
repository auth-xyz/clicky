#include "../include/clicky.hpp" 
#include <iostream>

int main(int argc, char* argv[]) {
    clicky parser;

    // Adding arguments
    parser.add_argument("operation", "", true, "Mathematical operation (add, subtract, multiply, divide)");
    parser.add_argument("x", "", true, "First number");
    parser.add_argument("y", "", true, "Second number");

    // Adding prefix 
    parser.set_prefix({"/"}, {"/"}); // new method

    // Parsing arguments
    parser.parse(argc, argv);

    // Retrieving arguments
    std::string operation = parser.argument("operation");
    double num1 = std::stod(parser.argument("x"));
    double num2 = std::stod(parser.argument("y"));

    // Performing operation
    if (operation == "add") {
        std::cout << (num1 + num2) << "\n";
    } else if (operation == "subtract") {
        std::cout << (num1 - num2) << "\n";
    } else if (operation == "multiply") {
        std::cout << (num1 * num2) << "\n";
    } else if (operation == "divide") {
        if (num2 == 0) {
            std::cerr << "Error: Division by zero\n";
            return 1;
        }
        std::cout << (num1 / num2) << "\n";
    } else {
        std::cerr << "Invalid operation: " << operation << "\n";
    }

    return 0;
}

// Usage:
// ./calculator --operation add --x 10 --y 10

