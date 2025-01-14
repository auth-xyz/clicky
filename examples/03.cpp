#include <clicky/clicky.hpp> 
#include <iostream>

int main(int argc, char* argv[]) {
    clicky cli;
    
    cli.add_argument("test", "t", "A test argument", false);
    cli.add_argument("test2", "t2", "Another test argument", false);

    cli.parse(argc, argv);

    try {
        std::string test = cli.get_argument("test");
        std::cout << "Test: " << test << "\n";

        std::string test2 = cli.get_argument("test2");
        std::cout << "Test2: " << test2 << "\n";

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }

    return 0;
}

