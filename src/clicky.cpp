// clicky.cpp
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

// ==== Parse Command-Line Arguments ====
void clicky::parse(int argc, char* argv[]) {
  for (int i = 1; i < argc; ++i) {
    std::string_view arg = argv[i];

    // Normalize argument
    if (arg.starts_with("--")) arg.remove_prefix(2);
    else if (arg.starts_with("-")) {
      std::string alias = std::string(arg.substr(1));
      arg = alias_map_.count(alias) ? alias_map_[alias] : alias;
    }

    if (flags_.count(arg.data())) {
      flags_[arg.data()].value = true;
    } else if (arguments_.count(arg.data())) {
      if (i + 1 < argc && argv[i + 1][0] != '-') {
        arguments_[arg.data()].value = argv[++i];
      } else if (arguments_[arg.data()].required) {
        missing_args_.push_back(arg.data());
      }
    } else {
      positional_args_.emplace_back(argv[i]);
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
  for (const auto& [name, arg] : arguments_) {
    if (arg.required && arg.value.empty()) {
      missing_args_.push_back(name);
      std::cerr << "Missing required argument: " << name << '\n';
    }
  }

  if (!missing_args_.empty()) {
    print_help();
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
      size_t length = name.length() + (item.alias.empty() ? 0 : item.alias.length() + 4);
      max_length = std::max(max_length, length);
    }
  };

  calculate_max_length(flags_);
  calculate_max_length(arguments_);

  std::cout << COLOR_BRIGHT_YELLOW << "Flags:\n" << COLOR_RESET;
  for (const auto& [name, flag] : flags_) {
    std::cout << COLOR_BRIGHT_CYAN << "  --" << name;
    if (!flag.alias.empty()) std::cout << COLOR_BRIGHT_GREEN << ", -" << flag.alias;

    size_t current_length = name.length() + (flag.alias.empty() ? 0 : flag.alias.length() + 4);
    size_t padding = max_length - current_length + 4;
    std::cout << std::string(padding, ' ') << COLOR_RESET << ": "
      << COLOR_WHITE << flag.description << COLOR_RESET
      << " (default: "
      << (flag.default_value ? COLOR_BRIGHT_GREEN "true" : COLOR_BRIGHT_RED "false")
      << COLOR_RESET << ")\n";
  }

  std::cout << "\n" << COLOR_BRIGHT_YELLOW << "Arguments:\n" << COLOR_RESET;
  for (const auto& [name, arg] : arguments_) {
    std::cout << COLOR_BRIGHT_CYAN << "  --" << name;
    if (!arg.alias.empty()) std::cout << COLOR_BRIGHT_GREEN << ", -" << arg.alias;

    size_t current_length = name.length() + (arg.alias.empty() ? 0 : arg.alias.length() + 4);
    size_t padding = max_length - current_length + 4;
    std::cout << std::string(padding, ' ') << COLOR_RESET << ": "
      << COLOR_WHITE << arg.description << COLOR_RESET
      << (arg.required ? COLOR_BRIGHT_RED " (required)" : COLOR_BRIGHT_GREEN " (optional)")
      << COLOR_RESET << '\n';
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
        std::cout << COLOR_BRIGHT_YELLOW << "Usage: \n" << COLOR_RESET
                  << COLOR_WHITE << "  " << formatted_usage << COLOR_RESET << "\n\n";
    }
}
