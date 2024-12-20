// clicky.cpp
#include "../include/clicky.hpp" 
#include <algorithm>

// ==== Constructor ====
clicky::clicky() {
  add_flag("help", "h", "Display this help message", false);
}

// ==== Add Argument ====
void clicky::add_argument(const std::string& name, const std::string& alias, bool required, const std::string& description) {
  arguments_[name] = {alias, required, description, ""};
  if (!alias.empty()) alias_map_[alias] = name;
}

// ==== Add Flag ====
void clicky::add_flag(const std::string& name, const std::string& alias, const std::string& description, bool default_value) {
  flags_[name] = {alias, default_value, description, false};
  if (!alias.empty()) alias_map_[alias] = name;
}

// ==== Parse Command-Line Arguments ====
void clicky::parse(int argc, char* argv[]) {
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];

    if (arg.starts_with("--")) arg = arg.substr(2);
    else if (arg.starts_with("-")) arg = alias_map_.count(arg.substr(1)) ? alias_map_[arg.substr(1)] : arg.substr(1);

    if (flags_.count(arg)) {
      flags_[arg].value = true;
    } else if (arguments_.count(arg)) {
      std::vector<std::string> values;
      while (i + 1 < argc && argv[i + 1][0] != '-') values.push_back(argv[++i]);
      if (!values.empty()) arguments_[arg].value = join_values(values);
      else if (arguments_[arg].required) missing_args_.push_back(arg);
    } else {
      positional_args_.push_back(argv[i]);
    }
  }

  if (flag("help")) {
    print_help();
    exit(0);
  }

  for (const auto& [name, arg] : arguments_) {
    if (arg.required && arg.value.empty()) {
      std::cerr << "Missing required argument: " << name << '\n';
      missing_args_.push_back(name);
    }
  }

  if (!missing_args_.empty()) {
    print_help();
    exit(1);
  }
}

// ==== Flag Value ====
bool clicky::flag(const std::string& name) const {
  return flags_.at(name).value;
}

// ==== Argument Value ====
std::string clicky::argument(const std::string& name) const {
  return arguments_.at(name).value;
}

// ==== Positional Arguments ====
std::vector<std::string> clicky::positional_arguments() const {
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

  std::cout << "Flags:\n";
  for (const auto& [name, flag] : flags_) {
    std::cout << "  --" << name;
    if (!flag.alias.empty()) std::cout << ", -" << flag.alias;

    size_t current_length = name.length() + (flag.alias.empty() ? 0 : flag.alias.length() + 4);
    size_t padding = max_length - current_length + 4;
    std::cout << std::string(padding, ' ') << ": " << flag.description
              << " (default: " << (flag.default_value ? "true" : "false") << ")\n";
  }

  std::cout << "\nArguments:\n";
  for (const auto& [name, arg] : arguments_) {
    std::cout << "  --" << name;
    if (!arg.alias.empty()) std::cout << ", -" << arg.alias;

    size_t current_length = name.length() + (arg.alias.empty() ? 0 : arg.alias.length() + 4);
    size_t padding = max_length - current_length + 4;
    std::cout << std::string(padding, ' ') << ": " << arg.description
              << (arg.required ? " (required)" : " (optional)") << '\n';
  }
}

