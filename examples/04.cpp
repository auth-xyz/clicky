#include <clicky/clicky.hpp> 
#include <iostream>

int main(int argc, char* argv[]) {
    clicky cli;
    
    cli.add_argument("test", "t", "A test argument", false);
    cli.add_argument("test2", "t2", "Another test argument", false);

    cli.add_options({
      {"verbose", "v", "Enable verbose output", false},
      {"overwrite", "c", "Overwrite existing files", false}
    });

    cli.group("options", {"v", "c"});
    cli.set_prefix({":"}, {":"});
    cli.parse(argc, argv);

    try {
        // Fetching the values of the arguments
        std::string test = cli.get_argument("test");
        std::cout << "Test: " << test << "\n";

        std::string test2 = cli.get_argument("test2");
        std::cout << "Test2: " << test2 << "\n";

        std::cout << "Verbose: " << cli.get_option("verbose") << "\n";
        std::cout << "Overwrite: " << cli.get_option("overwrite") << "\n";

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }

    return 0;
}


