#include "../include/clicky.hpp" 

int main(int argc, char* argv[]) {
  clicky cli;

  cli.add_arguments({
    {"test", "t", false, "test"},
    {"test2", "t2", false, "test2"}
  });

  cli.parse(argc,argv);
}
