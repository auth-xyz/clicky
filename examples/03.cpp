#include "../include/clicky.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  clicky cli;
  cli.add_arguments(
      {{"test", "t", false, "test"}, {"test2", "t2", false, "test2"}});

  try {
    std::string test = cli.argument("test");
    std::cout << test << "\n";

    std::string test2 = cli.argument("test2");
    std::cout << test2 << "\n";

  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << "\n";
  }

  cli.parse(argc, argv);
}
