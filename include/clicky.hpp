#pragma once

// ============= COLORS ==================
namespace cl_colors {
constexpr const char *RESET = "\033[0m";
constexpr const char *WHITE = "\033[37m";
constexpr const char *BRIGHT_RED = "\033[91m";
constexpr const char *BRIGHT_GREEN = "\033[92m";
constexpr const char *BRIGHT_YELLOW = "\033[93m";
constexpr const char *BRIGHT_CYAN = "\033[96m";
} // namespace cl_colors
// =======================================

#include <string>
#include <unordered_map>
#include <vector>

class clicky {
public:
  explicit clicky(const std::string &usage = "");

  // Add individual arguments and options
  void add_argument(const std::string &argument_name, const std::string &alias,
                    const std::string &description, bool required);
  void bulk_add_arguments(
      const std::vector<std::tuple<std::string, std::string, std::string, bool>>
          &args);

  void add_option(const std::string &opt_name, const std::string &alias,
                  const std::string &description, bool default_value);
  void bulk_add_options(
      const std::vector<std::tuple<std::string, std::string, std::string, bool>>
          &options);

  // Group options for combined usage like `-xyz`
  void group(const std::string &group_name,
             const std::vector<std::string> &option_aliases);

  // Parse input and retrieve values
  void parse(int argc, char *argv[]);
  const std::vector<std::string> &positional_arguments() const;

  std::string get_argument(const std::string &name) const;
  bool has_option(const std::string &name) const;
  bool has_argument(const std::string &name) const;

  // Help and configuration
  void print_help() const;
  void set_prefix(const std::vector<std::string> &argument_prefixes,
                  const std::vector<std::string> &option_prefixes = {});

  void enable_grouping();
  void disable_grouping();
  void set_color(bool state);

private:
  struct Option {
    bool default_value;
    std::string alias;
    std::string description;
    bool value;
  };

  struct Argument { 
    std::string description;
    std::string alias;
    std::string value;
    bool required;
  };

  // Internal state
  std::string usage_;
  bool color_state_ = true;
  bool grouping_enabled_ = false;

  std::vector<std::string> argument_prefixes_ = {"--", "-"};
  std::vector<std::string> option_prefixes_ = {"--", "-"};

  int argc_;
  char **argv_;

  // Mappings
  std::unordered_map<std::string, Option *> options_map_;
  std::unordered_map<std::string, Argument *> arguments_map_;
  std::unordered_map<std::string, std::string> short_name_map_;
  std::unordered_map<std::string, std::vector<std::string>> option_groups_;

  // Parsed state
  std::vector<std::string> missing_arguments_;
  std::vector<std::string> positional_arguments_;

  // Internal helper methods
  std::string join_values(const std::vector<std::string> &values) const;
  int parse_field(std::string arg);
  bool parse_set(std::string, std::string next_field = "");
  void validate_required_arguments();
  size_t calculate_max_length() const;

  // Finally implementing the TTY validation
  std::string get_color(const std::string &color) const;

  template <typename T>
  void print_items(const std::unordered_map<std::string, T *> &items,
                   size_t max_length) const;
  void print_usage(const std::string &program_name) const;
};
