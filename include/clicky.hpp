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

    void add_argument(const std::string& name, const std::string& alias, bool required, const std::string& description);
    void add_arguments(const std::vector<std::tuple<std::string, std::string, bool, std::string>>& args);
  
    void add_flag(const std::string& name, const std::string& alias = "", bool default_value = false, const std::string& description = "");
    void add_flags(const std::vector<std::tuple<std::string, std::string, bool, std::string>>& args);

    void parse(int argc, char* argv[]);
    bool flag(const std::string& name) const;
    std::string argument(const std::string& name) const;
    const std::vector<std::string>& positional_arguments() const;

    void print_help() const;
    void set_prefix(const std::vector<std::string>& arg_prefixes, const std::vector<std::string>& flag_prefixes = {});

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

    std::string usage_;
    std::vector<std::string> arg_prefixes_ = {"--", "-"}; 
    std::vector<std::string> flag_prefixes_ = {"--", "-"};  

    std::unordered_map<std::string, Flag> flags_;
    std::unordered_map<std::string, Argument> arguments_;
    std::unordered_map<std::string, std::string> alias_map_;

    std::vector<std::string> missing_args_;
    std::vector<std::string> positional_args_;

    std::string join_values(const std::vector<std::string>& values) const;
    int parse_field(std::string arg);

    void validate_required_arguments(); // Added
    size_t calculate_max_length() const; // Added

    template <typename T>
    void print_items(const std::unordered_map<std::string, T>& items, size_t max_length) const; // Added
    void print_usage(const std::string& program_name) const;
};


