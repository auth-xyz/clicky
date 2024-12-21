#ifndef CLICKY_HPP
#define CLICKY_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <numeric>
#include <iostream>

class clicky {
public:
  clicky();

  void add_argument(const std::string& name, const std::string& alias, bool required, const std::string& description);
  void add_arguments(const std::vector<std::tuple<std::string, std::string, bool, std::string>>& args);
  void add_flag(const std::string& name, const std::string& alias = "", bool default_value = false, const std::string& description = "");

  void parse(int argc, char* argv[]);

  bool flag(const std::string& name) const;
  std::string argument(const std::string& name) const;
  std::vector<std::string> positional_arguments() const;

private:
  struct Flag {
    std::string alias;
    bool default_value;
    std::string description;
    bool value;
  };

  struct Argument {
    std::string alias;
    bool required;
    std::string description;
    std::string value;
  };

  std::unordered_map<std::string, Flag> flags_;
  std::unordered_map<std::string, Argument> arguments_;
  std::unordered_map<std::string, std::string> alias_map_;

  std::vector<std::string> missing_args_;
  std::vector<std::string> positional_args_;

  std::string join_values(const std::vector<std::string>& values) const;
  void print_help() const;
};

#endif // CLICKY_HPP

