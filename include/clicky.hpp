#pragma once 

// ============= COLORS ==================
namespace cl_colors {
    constexpr const char* RESET = "\033[0m";
    constexpr const char* WHITE = "\033[37m";
    constexpr const char* BRIGHT_RED = "\033[91m";
    constexpr const char* BRIGHT_GREEN = "\033[92m";
    constexpr const char* BRIGHT_YELLOW = "\033[93m";
    constexpr const char* BRIGHT_CYAN = "\033[96m";
}
// =======================================

#include <string>
#include <vector>
#include <unordered_map>

class clicky {
public:
    explicit clicky(const std::string& usage = "");

    void add_argument(const std::vector<std::string> args_long, const std::vector<std::string> args_short, bool required, const std::string& description);
    void add_arguments(const std::vector<std::tuple<std::vector<std::string>, std::vector<std::string>, bool, std::string>>& args);
  
    void add_option(const std::vector<std::string> options_long, const std::vector<std::string> options_short = {}, bool default_value = false, const std::string& description = "");
    void add_options(const std::vector<std::tuple<std::vector<std::string>, std::vector<std::string>, bool, std::string>>& args);

    void parse(int argc, char* argv[]);
    bool option(const std::string& name) const;
    std::string argument(const std::string& name) const;
    const std::vector<std::string>& positional_arguments() const;
    bool has_argument(const std::string& name) const;

    void print_help() const;
    void set_prefix(const std::vector<std::string>& arg_prefixes, const std::vector<std::string>& option_prefixes = {});

private:
    struct Option {
        bool default_value;
        std::string description;
        bool value;
    };

  struct Argument {
    bool required;
    std::string description;
    std::string value;                 // For single value
    std::vector<std::string> values;   // For multiple values
  };

  std::string usage_;
    std::vector<std::string> arg_prefixes_ = {"--", "-"}; 
    std::vector<std::string> option_prefixes_ = {"--", "-"};  

    int argc_;
    char** argv_;

    // TODO: get rid of alias_map_ or replace with something different
    std::unordered_map<std::string, std::string> alias_map_;

    std::unordered_map<std::string, Option*> options_long_;
    std::unordered_map<std::string, Option*> options_short_;
    std::unordered_map<std::string, Argument*> args_long_;
    std::unordered_map<std::string, Argument*> args_short_;

    std::vector<std::string> missing_args_;
    std::vector<std::string> positional_args_;

    std::string join_values(const std::vector<std::string>& values) const;
    int parse_field(std::string arg);
    bool parse_set(std::string, std::string next_field = "");

    void validate_required_arguments(); 
    size_t calculate_max_length() const; 

    template <typename T>
    void print_items(const std::unordered_map<std::string, T>& items, size_t max_length) const; 
    void print_usage(const std::string& program_name) const;
};


