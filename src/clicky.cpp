#include "../include/clicky.hpp"
#include <algorithm>
#include <iostream>
#include <numeric>
#include <regex>
#include <stdexcept>

// ==== cross-platform ====
#ifdef _WIN32
#include <io.h>
#define isatty _isatty
#define fileno _fileno
#else
#include <unistd.h>
#endif
// ========================

// ==== Constructor ====
clicky::clicky(const std::string &usage) : usage_(usage) {
  add_option("help", "h", "Display this help message", false);
}

// Add a Single Argument
void clicky::add_argument(const std::string &arg_name, const std::string &alias,
                          const std::string &description, bool required) {
  auto *arg = new Argument{required, description, "", {}};
  args_map_[arg_name] = arg;
  if (!alias.empty()) {
    alias_map_[alias] = arg_name;
    args_map_[alias] = arg;
  }
}

// Add Multiple Arguments
void clicky::add_arguments(
    const std::vector<std::tuple<std::string, std::string, std::string, bool>>
        &args) {
  for (const auto &[arg_name, alias, description, required] : args) {
    add_argument(arg_name, alias, description, required);
  }
}

// Add a Single Option
void clicky::add_option(const std::string &opt_name, const std::string &alias,
                        const std::string &description, bool default_value) {
  auto *opt = new Option{default_value, alias, description, false};
  options_map_[opt_name] = opt;
  if (!alias.empty()) {
    alias_map_[alias] = opt_name;
    options_map_[alias] = opt;
  }
}

// Add Multiple Options
void clicky::add_options(
    const std::vector<std::tuple<std::string, std::string, std::string, bool>>
        &options) {
  for (const auto &[opt_name, alias, description, default_value] : options) {
    add_option(opt_name, alias, description, default_value);
  }
}

// Group Options
void clicky::group(const std::string &group_name,
                   const std::vector<std::string> &option_aliases) {
  option_groups_[group_name] = {};
  enable_grouping();
  for (const auto &alias : option_aliases) {
    if (alias_map_.count(alias)) {
      option_groups_[group_name].push_back(alias);
    } else {
      throw std::invalid_argument("Invalid option alias for grouping: " +
                                  alias);
    }
  }
}

void clicky::enable_grouping() { this->grouping_enabled_ = true; }

void clicky::disable_grouping() { this->grouping_enabled_ = false; }

// ==== Set Prefix ====
void clicky::set_prefix(const std::vector<std::string> &arg_prefixes,
                        const std::vector<std::string> &option_prefixes) {
  arg_prefixes_ = arg_prefixes;
  option_prefixes_ = option_prefixes.empty() ? arg_prefixes : option_prefixes;
}

// ==== Parse One Field ====
int clicky::parse_field(std::string field) {
  std::string original_field = field; // Keep original for reference

  if (options_map_.count(field)) {
    options_map_[field]->value = true;
    return 0;
  }

  // Handle key-value pairs (e.g., --arg=value)
  size_t value_separator = field.find('=');
  if (value_separator != std::string::npos) {
    std::string key = field.substr(0, value_separator);
    std::string value = field.substr(value_separator + 1);

    if (args_map_.count(key)) {
      args_map_[key]->value = value;
    } else {
      std::cerr << "Unknown argument: " << original_field << '\n';
      return 1;
    }
  } else {
    positional_args_.emplace_back(field);
  }

  return 0;
}

// ==== Parse A Concatenated Set Of Flags ====
bool clicky::parse_set(std::string field, std::string next_field) {
  bool next_field_used = false;
  std::cout << field << " " << next_field << '\n';

  // Check if grouping is enabled
  if (!grouping_enabled_) {
    if (alias_map_.count(field)) {
      std::string expanded = alias_map_[field];

      if (args_map_.count(expanded)) {
        if (!next_field_used && !next_field.empty()) {
          args_map_[expanded]->value = next_field;
          next_field_used = true;
        } else {
          std::cerr << "Argument --" << field << " requires a value.\n";
          exit(1);
        }
      } else if (options_map_.count(expanded)) {
        options_map_[expanded]->value = true;
      } else {
        std::cerr << "Unknown option: --" << field << '\n';
        exit(1);
      }
    } else {
      std::cerr << "Unknown option: " << field << '\n';
      exit(1);
    }
  } else {
    for (char alias : field) {
      std::string alias_str(1, alias);

      if (alias_map_.count(alias_str)) {
        std::string expanded = alias_map_[alias_str];

        if (args_map_.count(expanded)) {
          if (!next_field_used && !next_field.empty()) {
            args_map_[expanded]->value = next_field;
            next_field_used = true;
          } else {
            std::cerr << "Argument -" << alias << " requires a value.\n";
            exit(1);
          }
        } else if (options_map_.count(expanded)) {
          options_map_[expanded]->value = true;
        } else {
          std::cerr << "Unknown alias: -" << alias << std::endl;
          exit(1);
        }
      } else {
        std::cerr << "Unknown alias: -" << alias << std::endl;
        exit(1);
      }
    }
  }

  return next_field_used;
}

// ==== Parse Command-Line Arguments ====
void clicky::parse(int argc, char *argv[]) {
  argv_ = argv;
  argc_ = argc;

  for (int i = 1; i < argc; ++i) {
    std::string field = argv[i];
    std::string original_field = field; 
    bool is_concatenated_flags = false;

    // Remove prefixes for options and arguments
    for (const auto &prefix : option_prefixes_) {
      if (field.starts_with(prefix)) {
        field = field.substr(prefix.length());
        break;
      }
    }

    for (const auto &prefix : arg_prefixes_) {
      if (field.starts_with(prefix)) {
        field = field.substr(prefix.length());
        break;
      }
    }

    if (options_map_.count(field)) {
      options_map_[field]->value = true; // Long-form option matched
    } else if (args_map_.count(field)) {
      if (i + 1 < argc &&
          !std::string(argv[i + 1]).starts_with(option_prefixes_[0]) &&
          !std::string(argv[i + 1]).starts_with(arg_prefixes_[0])) {
        args_map_[field]->value = argv[++i];
      } else {
        std::cerr << "Argument --" << field << " requires a value.\n";
        exit(1);
      }
    } else {
      // Determine if it's a concatenated set of flags
      if (field.length() > 1 && !args_map_.count(field)) {
        is_concatenated_flags = true;
      }

      if (is_concatenated_flags) {
        // Process concatenated flags
        if (parse_set(field, (i + 1 < argc) ? argv[i + 1] : "")) {
          ++i;
        }
      } else {
        std::cerr << "Unknown option: " << original_field << '\n';
        exit(1);
      }
    }
  }

  if (has_option("help")) {
    print_usage(argv[0]);
    print_help();
    exit(0);
  }

  validate_required_arguments();
}

// ==== Validate Required Arguments ====
void clicky::validate_required_arguments() {
  std::stringstream error_message;

  for (const auto &[name, arg] : args_map_) {
    if (arg->required && arg->value.empty()) {
      missing_args_.push_back(name);
      error_message << cl_colors::BRIGHT_CYAN << name << cl_colors::RESET
                    << " : " << arg->description << " [required]\n";
    }
  }

  if (!missing_args_.empty()) {
    std::cerr << cl_colors::BRIGHT_RED << "Missing required argument(s):\n"
              << cl_colors::RESET << error_message.str();
    exit(1);
  }
}

// ==== Option Value ====
bool clicky::has_option(const std::string &name) const {
  auto it = options_map_.find(name);
  return it != options_map_.end() && it->second->value;
}

// ==== Argument Value ====
std::string clicky::get_argument(const std::string &name) const {
  auto it = args_map_.find(name);
  if (it == args_map_.end() || it->second->value.empty()) {
    throw std::out_of_range("Argument '" + name +
                            "' is missing or not provided.");
  }
  return it->second->value;
}

// ==== Has Argument ====
bool clicky::has_argument(const std::string &name) const {
  return args_map_.count(name) && !args_map_.at(name)->value.empty();
}

// ==== Positional Arguments ====
const std::vector<std::string> &clicky::positional_arguments() const {
  return positional_args_;
}

// ==== Join Values ====
std::string clicky::join_values(const std::vector<std::string> &values) const {
  return std::accumulate(values.begin(), values.end(), std::string(),
                         [](const std::string &a, const std::string &b) {
                           return a.empty() ? b : a + "\n" + b;
                         });
}

// ==== Print Help Message ====
void clicky::print_help() const {
  size_t max_length = calculate_max_length();

  std::cout << get_color(cl_colors::BRIGHT_YELLOW) << "Options:\n"
            << get_color(cl_colors::RESET);
  print_items(options_map_, max_length);

  std::cout << "\n" << get_color(cl_colors::BRIGHT_YELLOW) << "Arguments:\n"
            << get_color(cl_colors::RESET);
  print_items(args_map_, max_length);
}

// ==== Print Items Helper Function ====
template <typename T>
void clicky::print_items(const std::unordered_map<std::string, T *> &items,
                         size_t max_length) const {
  for (const auto &[name, item] : items) {
    if (name.length() == 1) {
      continue;
    }

    std::cout << "  " << get_color(cl_colors::BRIGHT_CYAN) << "--" << name;

    if (!item->alias.empty()) {
      std::cout << get_color(cl_colors::BRIGHT_GREEN) << ", -" << item->alias;
    }

    size_t current_length =
        name.length() + (item->alias.empty() ? 0 : item->alias.length() + 4);
    size_t padding = max_length - current_length + 4;

    std::cout << std::string(padding, ' ') << get_color(cl_colors::RESET) << ": "
              << get_color(cl_colors::WHITE) << item->description
              << get_color(cl_colors::RESET);

    if constexpr (std::is_same_v<T, Option>) {
      std::cout << " (default: "
                << (item->default_value
                        ? (get_color(cl_colors::BRIGHT_GREEN) + "true")
                        : (get_color(cl_colors::BRIGHT_RED) + "false"))
                << get_color(cl_colors::RESET) << ")";
    } else {
      std::cout << (item->required
                        ? get_color(cl_colors::BRIGHT_RED) + " (required)"
                        : get_color(cl_colors::BRIGHT_GREEN) + " (optional)")
                << get_color(cl_colors::RESET);
    }

    std::cout << '\n';
  }
}
size_t clicky::calculate_max_length() const {
  size_t max_length = 0;

  auto update_max_length = [&](const auto &map) {
    for (const auto &[name, item] : map) {
      if (name.length() == 1) {
        continue;
      }

      size_t length = name.length();
      if (!item->alias.empty()) {
        length += item->alias.length() + 4; // Account for ", -"
      }
      max_length = std::max(max_length, length);
    }
  };

  update_max_length(options_map_);
  update_max_length(args_map_);

  return max_length;
}

void clicky::print_usage(const std::string &program_name) const {
  if (!usage_.empty()) {
    std::string formatted_usage =
        std::regex_replace(usage_, std::regex("\\{program\\}"), program_name);
    std::cout << get_color(cl_colors::BRIGHT_YELLOW) << "Usage: \n"
              << get_color(cl_colors::RESET) << get_color(cl_colors::WHITE)
              << "  " << formatted_usage << get_color(cl_colors::RESET)
              << "\n\n";
  }
}

std::string clicky::get_color(const std::string &color) const {
  if (color_state_ && isatty(STDOUT_FILENO)) {
    return color;
  }
  return ""; // No colors
}

void clicky::set_color(bool state) {
  this->color_state_ = state;
}
