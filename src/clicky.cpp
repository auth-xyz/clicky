#include "../include/clicky.hpp"
#include <algorithm>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <regex>

// ==== Constructor ====
clicky::clicky(const std::string& usage) : usage_(usage) {
  add_option("help", "h", false, "Display this help message");
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

// ==== Add Option ====
void clicky::add_option(const std::string& name, const std::string& alias, bool default_value, const std::string& description) {
    options_[name] = {alias, default_value, description, false};
    if (!alias.empty()) alias_map_[alias] = name;
}

// ==== Add Multiple Options ====
void clicky::add_options(const std::vector<std::tuple<std::string, std::string, bool, std::string>>& options) {
    for (const auto& [name, alias, default_value, description] : options) {
        add_option(name, alias, default_value, description);
    }
}

// ==== Set Prefix ====
void clicky::set_prefix(const std::vector<std::string>& arg_prefixes, const std::vector<std::string>& option_prefixes) {
    arg_prefixes_ = arg_prefixes;
    option_prefixes_ = option_prefixes.empty() ? arg_prefixes : option_prefixes;
}

// ==== Parse One Field ====
int clicky::parse_field(std::string field) {

  if (options_.count(field)) {
    options_[field].value = true;
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

bool clicky::parse_set(std::string field, std::string next_field) {
    bool next_field_used = false;
    for (size_t fi = 0; fi < field.length(); ++fi) {
        std::string expanded;

        if (alias_map_.count(std::string(1, field[fi]))) {
            expanded = alias_map_[std::string(1, field[fi])];
        } else {
            std::cerr << "Unknown alias: -" << field[fi] << std::endl;
            print_usage(argv_[0]);
            print_help();
            exit(0);
        }

        if (arguments_.count(expanded)) {
            if (fi + 1 >= field.length()) {
                expanded += " " + std::string(next_field);
                next_field_used = true;
            } else {
                expanded += " " + field.substr(fi + 1);
            }
            parse_field(expanded);
            break;
        }
        parse_field(expanded);
    }
    return next_field_used;
}

// ==== Parse Command-Line Arguments ====
void clicky::parse(int argc, char* argv[]) {
    argv_ = argv;
    argc_ = argc;

    for (int i = 1; i < argc; ++i) {
        std::string field = argv[i];
        bool is_alias = false;

        if (!option_prefixes_.empty() && field.starts_with(option_prefixes_[0])) {
          field = field.substr(option_prefixes_[0].length());
        } else if (!arg_prefixes_.empty() && field.starts_with(arg_prefixes_[0])) {
          field = field.substr(arg_prefixes_[0].length());
        } else is_alias = true;

        if (!is_alias) {

          if (arguments_.count(field)) {
            if (i + 1 < argc
                && !std::string(argv[i + 1]).starts_with(option_prefixes_[0])
                && !std::string(argv[i + 1]).starts_with(arg_prefixes_[0])) {
              field += " " + std::string(argv[++i]);
            } else if (arguments_[field].required) {
              missing_args_.push_back(field);
            }
          }
          parse_field(field);

        } else {
            if (!option_prefixes_.empty() && field.starts_with(option_prefixes_[1])) {
                field = field.substr(option_prefixes_[1].length());
            } else if (!arg_prefixes_.empty() && field.starts_with(arg_prefixes_[1])) {
                field = field.substr(arg_prefixes_[1].length());
            }

            if (
                    parse_set(field,
                        i + 1 < argc_
                        && !std::string(argv_[i + 1]).starts_with(option_prefixes_[0])
                        && !std::string(argv_[i + 1]).starts_with(option_prefixes_[1])
                        && !std::string(argv_[i + 1]).starts_with(arg_prefixes_[0])
                        && !std::string(argv_[i + 1]).starts_with(arg_prefixes_[1])
                        ? argv_[i + 1] : "")
               ) ++i;

        }
    }

    if (option("help")) {
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

// ==== Option Value ====
bool clicky::option(const std::string& name) const {
  auto it = options_.find(name);
  return it != options_.end() && it->second.value;
}

// ==== Argument Value ====
std::string clicky::argument(const std::string& name) const {
  auto it = arguments_.find(name);
  if (it == arguments_.end() || it->second.value.empty()) {
    throw std::out_of_range("Argument '" + name + "' is missing or not provided.");
  }
  return it->second.value;
}

// ==== Has Argument ====
bool clicky::has_argument(const std::string& name) const {
    return arguments_.count(name) && !arguments_.at(name).value.empty();
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

    // Lambda to calculate maximum length of arguments/options for formatting
    auto calculate_max_length = [&](const auto& map) {
        for (const auto& [name, item] : map) {
            size_t length = name.length();
            if (!item.alias.empty()) {
                for (const auto& prefix : option_prefixes_) {
                    length = std::max(length, name.length() + prefix.length() + 2); // Account for prefix and ", "
                }
            }
            max_length = std::max(max_length, length);
        }
    };

    calculate_max_length(options_);
    calculate_max_length(arguments_);

    std::cout << cl_colors::BRIGHT_YELLOW << "Options:\n" << cl_colors::RESET;
    for (const auto& [name, option] : options_) {
        for (const auto& prefix : option_prefixes_) {
            std::cout << cl_colors::BRIGHT_CYAN << "  " << prefix << name;
            if (!option.alias.empty()) {
                std::cout << cl_colors::BRIGHT_GREEN << ", " << prefix << option.alias;
            }

            size_t current_length = name.length() + (option.alias.empty() ? 0 : option.alias.length() + prefix.length() + 2);
            size_t padding = max_length - current_length + 4;
            std::cout << std::string(padding, ' ') << cl_colors::RESET << ": "
                      << cl_colors::WHITE << option.description << cl_colors::RESET
                      << " (default: "
                      << (option.default_value ? (cl_colors::BRIGHT_GREEN + std::string("true")) : (cl_colors::BRIGHT_RED + std::string("false")))
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

    if constexpr (std::is_same_v<T, Option>) {
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

  update_max_length(options_);
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
