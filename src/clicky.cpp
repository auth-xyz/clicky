// clicky.cpp
#include "../include/clicky.hpp" 
#include <algorithm>

// ==== Constructor ====
clicky::clicky() {
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
void clicky::add_flags(const std::vector<std::tuple<std::string, std::string, bool, std::string>>& args) {
  for (const auto& [name,alias,default_value, description] : args) {
    add_flag(name,alias,default_value,description);
  }
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
      if (i + 1 < argc && argv[i + 1][0] != '-') {
        arguments_[arg].value = argv[++i];
      } else if (arguments_[arg].required) {
        missing_args_.push_back(arg);
      }
    } else {
      positional_args_.push_back(argv[i]);
    }
  }

  if (flag("help")) {
    print_help();
    exit(0);
  }

  // Check for required arguments
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
  if (it == flags_.end()) return false; // Default to false if the flag doesn't exist
  return it->second.value;
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

