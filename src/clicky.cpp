#include "../include/clicky.hpp"
#include <algorithm>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <regex>

// ==== Constructor ====
clicky::clicky(const std::string& usage) : usage_(usage) {
  add_flag("help", "h", false, "Display this help message");
}

// ==== Add Argument ====
void clicky::add_argument(const std::string& name, const std::string& alias, bool required, const std::string& description) {
    arguments_[name] = {alias, required, description, ""};
    if (!alias.empty()) alias_map_[alias] = name;
}

// ==== Add Multiple Arguments ====
void clicky::add_arguments(const std::vector<std::tuple<std::string, std::string, bool, std::string>>& args) {
    for (const auto& [name, alias, required, description] : args) {
        add_argument(name, alias, required, description);
    }
}

// ==== Add Flag ====
void clicky::add_flag(const std::string& name, const std::string& alias, bool default_value, const std::string& description) {
    flags_[name] = {alias, default_value, description, false};
    if (!alias.empty()) alias_map_[alias] = name;
}

// ==== Add Multiple Flags ====
void clicky::add_flags(const std::vector<std::tuple<std::string, std::string, bool, std::string>>& flags) {
    for (const auto& [name, alias, default_value, description] : flags) {
        add_flag(name, alias, default_value, description);
    }
}

// ==== Set Prefix ====
void clicky::set_prefix(const std::vector<std::string>& arg_prefixes, const std::vector<std::string>& flag_prefixes) {
    arg_prefixes_ = arg_prefixes;
    flag_prefixes_ = flag_prefixes.empty() ? arg_prefixes : flag_prefixes;
}

// ==== Parse One Field ====
int clicky::parse_field(std::string field) {

  if (flags_.count(field)) {
    flags_[field].value = true;
    return 0;
  }

  size_t value_separator = field.find('=');
  value_separator == std::string::npos
    && (value_separator = field.find(' '));

  if (value_separator != std::string::npos) {
    std::string key = field.substr(0, value_separator);
    std::string value = field.substr(value_separator + 1);

    if (arguments_.count(key)) {
      arguments_[key].value = value;
    } else {
      std::cerr << "Unknown argument: " << key << '\n';
      return 1;
    }
  } else {
    positional_args_.emplace_back(field);
  }

  return 0;
}

// ==== Parse Command-Line Arguments ====
void clicky::parse(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string field = argv[i];
        bool is_alias = false;

        if (!flag_prefixes_.empty() && field.starts_with(flag_prefixes_[0])) {
          field = field.substr(flag_prefixes_[0].length());
        } else if (!arg_prefixes_.empty() && field.starts_with(arg_prefixes_[0])) {
          field = field.substr(arg_prefixes_[0].length());
        } else is_alias = true;

        if (!is_alias) {

          if (arguments_.count(field)) {
            if (i + 1 < argc
                && !std::string(argv[i + 1]).starts_with(flag_prefixes_[0])
                && !std::string(argv[i + 1]).starts_with(arg_prefixes_[0])) {
              field += " " + std::string(argv[++i]);
            } else if (arguments_[field].required) {
              missing_args_.push_back(field);
            }
          }
          parse_field(field);

        } else {

          if (!flag_prefixes_.empty() && field.starts_with(flag_prefixes_[1])) {
            field = field.substr(flag_prefixes_[1].length());
          } else if (!arg_prefixes_.empty() && field.starts_with(arg_prefixes_[1])) {
            field = field.substr(arg_prefixes_[1].length());
          }
          
          for (size_t fi = 0; fi < field.length(); ++fi) {
            std::string expanded;

            if (alias_map_.count(std::string(1, field[fi]))) {
              expanded = alias_map_[std::string(1, field[fi])];
            } else {
              std::cerr << "Unknown alias: -" << field[fi] << std::endl;
              print_usage(argv[0]);
              print_help();
              exit(0);
            }

            if (arguments_.count(expanded)) {
              if (fi + 1 >= field.length()) {
                expanded += " " + std::string(argv[++i]);
              } else {
                expanded += " " + field.substr(fi + 1);
              }
              parse_field(expanded);
              break;
            }
            parse_field(expanded);
          }

        }
    }

    if (flag("help")) {
        print_usage(argv[0]);
        print_help();
        exit(0);
    }

    validate_required_arguments();
}

void clicky::validate_required_arguments() {
  std::stringstream error_message;

  for (const auto& [name, arg] : arguments_) {
    if (arg.required && arg.value.empty()) {
      missing_args_.push_back(name);
      error_message << (cl_colors::BRIGHT_CYAN + name + cl_colors::RESET)
                    << " : " << arg.description << " [required]\n";
    }
  }

  if (!missing_args_.empty()) {
    std::cerr << cl_colors::BRIGHT_RED
              << "Missing required argument(s):\n"
              << cl_colors::RESET
              << error_message.str();
    exit(1);
  }
}

// ==== Flag Value ====
bool clicky::flag(const std::string& name) const {
  auto it = flags_.find(name);
  return it != flags_.end() && it->second.value;
}

// ==== Argument Value ====
std::string clicky::argument(const std::string& name) const {
  auto it = arguments_.find(name);
  if (it == arguments_.end() || it->second.value.empty()) {
    throw std::out_of_range("Argument '" + name + "' is missing or not provided.");
  }
  return it->second.value;
}

// ==== Positional Arguments ====
const std::vector<std::string>& clicky::positional_arguments() const {
  return positional_args_;
}

// ==== Join Values ====
std::string clicky::join_values(const std::vector<std::string>& values) const {
  return std::accumulate(values.begin(), values.end(), std::string(),
                         [](const std::string& a, const std::string& b) {
                         return a.empty() ? b : a + "\n" + b;
                         });
}

// ==== Print Help Message ====
void clicky::print_help() const {
    size_t max_length = 0;

    // Lambda to calculate maximum length of arguments/flags for formatting
    auto calculate_max_length = [&](const auto& map) {
        for (const auto& [name, item] : map) {
            size_t length = name.length();
            if (!item.alias.empty()) {
                for (const auto& prefix : flag_prefixes_) {
                    length = std::max(length, name.length() + prefix.length() + 2); // Account for prefix and ", "
                }
            }
            max_length = std::max(max_length, length);
        }
    };

    calculate_max_length(flags_);
    calculate_max_length(arguments_);

    std::cout << cl_colors::BRIGHT_YELLOW << "Flags:\n" << cl_colors::RESET;
    for (const auto& [name, flag] : flags_) {
        for (const auto& prefix : flag_prefixes_) {
            std::cout << cl_colors::BRIGHT_CYAN << "  " << prefix << name;
            if (!flag.alias.empty()) {
                std::cout << cl_colors::BRIGHT_GREEN << ", " << prefix << flag.alias;
            }

            size_t current_length = name.length() + (flag.alias.empty() ? 0 : flag.alias.length() + prefix.length() + 2);
            size_t padding = max_length - current_length + 4;
            std::cout << std::string(padding, ' ') << cl_colors::RESET << ": "
                      << cl_colors::WHITE << flag.description << cl_colors::RESET
                      << " (default: "
                      << (flag.default_value ? (cl_colors::BRIGHT_GREEN + std::string("true")) : (cl_colors::BRIGHT_RED + std::string("false")))
                      << cl_colors::RESET << ")\n";
        }
    }

    std::cout << "\n" << cl_colors::BRIGHT_YELLOW << "Arguments:\n" << cl_colors::RESET;
    for (const auto& [name, arg] : arguments_) {
        for (const auto& prefix : arg_prefixes_) {
            std::cout << cl_colors::BRIGHT_CYAN << "  " << prefix << name;
            if (!arg.alias.empty()) {
                std::cout << cl_colors::BRIGHT_GREEN << ", " << prefix << arg.alias;
            }

            size_t current_length = name.length() + (arg.alias.empty() ? 0 : arg.alias.length() + prefix.length() + 2);
            size_t padding = max_length - current_length + 4;
            std::cout << std::string(padding, ' ') << cl_colors::RESET << ": "
                      << cl_colors::WHITE << arg.description << cl_colors::RESET
                      << (arg.required ? (cl_colors::BRIGHT_RED + std::string(" (required)")) : (cl_colors::BRIGHT_GREEN + std::string(" (optional)")))
                      << cl_colors::RESET << '\n';
        }
    }
}

template <typename T>
void clicky::print_items(const std::unordered_map<std::string, T>& items, size_t max_length) const {
  for (const auto& [name, item] : items) {
    std::cout << "  --" << name;
    if (!item.alias.empty()) std::cout << ", -" << item.alias;

    size_t current_length = name.length() + (item.alias.empty() ? 0 : item.alias.length() + 4);
    size_t padding = max_length - current_length + 4;
    std::cout << std::string(padding, ' ') << ": " << item.description;

    if constexpr (std::is_same_v<T, Flag>) {
      std::cout << " (default: " << (item.default_value ? "true" : "false") << ")";
    } else {
      std::cout << (item.required ? " (required)" : " (optional)");
    }
    std::cout << '\n';
  }
}

size_t clicky::calculate_max_length() const {
  size_t max_length = 0;

  auto update_max_length = [&](const auto& map) {
    for (const auto& [name, item] : map) {
      size_t length = name.length() + (item.alias.empty() ? 0 : item.alias.length() + 4);
      max_length = std::max(max_length, length);
    }
  };

  update_max_length(flags_);
  update_max_length(arguments_);
  return max_length;
}

void clicky::print_usage(const std::string& program_name) const {
    if (!usage_.empty()) {
        std::string formatted_usage = std::regex_replace(usage_, std::regex("\\{program\\}"), program_name);
        std::cout << cl_colors::BRIGHT_YELLOW << "Usage: \n" << cl_colors::RESET
                  << cl_colors::WHITE << "  " << formatted_usage << cl_colors::RESET << "\n\n";
    }
}
