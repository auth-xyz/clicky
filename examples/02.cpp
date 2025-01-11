#include <clicky/clicky.hpp> 
#include <iostream>

int main(int argc, char* argv[]) {
    clicky parser("{program} [operation(add, subtract, multiply, divide)] [x] [y]");

    parser.add_argument("operation", "o", "Mathematical operation (add, subtract, multiply, divide)", true);
    parser.add_argument("x", "", "First number", true);
    parser.add_argument("y", "", "Second number", true);

    parser.add_options({
      {"verbose", "v", "Enable verbose output", false},
      {"overwrite", "c", "Overwrite existing files", false}
    });
    parser.group("options", {"v", "c"});


    parser.parse(argc, argv);

    std::string operation = parser.argument("operation");
    double num1 = std::stod(parser.argument("x"));
    double num2 = std::stod(parser.argument("y"));

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
        return 1;
    }

    return 0;
}

// Usage:
// ./calculator /operation add /x 10 /y 10

