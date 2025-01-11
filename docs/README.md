<p align="center">
    <img src="https://github.com/auth-xyz/assets/blob/main/logos/chunky.png?raw=true" alt="logo" width="400" height="400">
</p>

----

### CLIcky

**clicky** is a simple C++ library for handling command-line arguments and options. It allows you to define options and arguments, parse input, and display formatted help messages. You can easily add required or optional arguments, set default values for options, and access both positional and named arguments. The library supports aliases for options and arguments, ensures all required arguments are provided, and presents clear, color-coded help information for better usability.

----

Now, you might be asking me: "Auth, how do I install this marvelous creation?", and I am here to answer that question my friend!
You see, installing `clicky` will require you to do a bit of setting up, nothing out of the ordinary, just installing a package or two.
Below, is how you install the two *main* requirements to build `clicky`. Now, if you want the single-header file, which will skip these
steps below, then you can go to the [releases](https://github.com/auth-xyz/clicky/releases) page and download the single-header file.

The two tools you need to install `clicky` are:
- [meson](https://mesonbuild.com/)
- [ninja](https://ninja-build.org/)

That's it! And it's easy to install! Below is how you'd do using any dnf/yum based distros:

```bash
sudo dnf install -y meson ninja
```

After installing `meson` and `ninja`, you're ready to build, and later install, clicky!
```bash
git clone https://github.com/auth-xyz/clicky.git
cd clicky

meson setup build
ninja -C build
sudo ninja -C build install
```

That's literally it! You installed clicky successfully!
Now I need to put this reminder here, whenever you use clicky, you *need* to pass the -lclicky option to `g++`
*only if you're installed the library*

----

### Documentation

So, I'll try to make this documentation concise and easy to digest.
Below, you'll find a list of the methods that are under the public interface of `clicky`:

#### `clicky::clicky(std::string usage = "")` - **constructor**
- Takes an optional `std::string` argument that is the usage message for the program. The usage can include the special variable `{program}`, which will be replaced with `argv[0]`.

#### `clicky::add_argument(const std::string& name, const std::string& alias, const std::string& description, bool required)` - **method**
- Adds a positional argument. Takes 4 arguments:
  - `name`: The name of the argument.
  - `alias`: The alias for the argument.
  - `description`: A brief description of the argument.
  - `required`: A boolean indicating whether the argument is required.

  The `clicky::add_arguments()` method is a convenience method for adding multiple arguments at once.

#### `clicky::add_option(const std::string& name, const std::string& alias, const std::string& description, bool default_value)` - **method**
- Adds an option. Takes 4 arguments:
  - `name`: The name of the option.
  - `alias`: The alias for the option.
  - `description`: A brief description of the option.
  - `default_value`: A boolean indicating the value of the option. 

  The `clicky::add_options()` method is a convenience method for adding multiple options at once.

#### `clicky::get_argument(const std::string& name)` - **method**
- Returns a pointer to the argument with the given name. If the argument is not found, returns `nullptr`.

#### `clicky::has_argument(const std::string& name)` - **method**
- Returns a boolean indicating whether the argument with the given name exists.

#### `clicky::get_option(const std::string& name)` - **method**
- Returns a pointer to the option with the given name. If the option is not found, returns `false`.

#### `clicky::set_prefix(const std::vector<std::string>& arg_prefixes, const std::vector<std::string>& option_prefixes)` - **method**
- Sets prefixes for arguments and options. The `arg_prefixes` vector applies to argument prefixes, while `option_prefixes` applies to option prefixes. This allows you to customize how arguments and options are prefixed (e.g., using `-` or `/` for flags).

#### `clicky::group(const std::string& group_name, const std::vector<std::string>& option_aliases)` - **method**
- Groups options together. The `group_name` is the name of the group, and `option_aliases` is a vector of option aliases that belong to the group. Allows things like ./someprogram -xyz

#### `clicky::parse(int argc, char* argv[])` - **method**
- Parses the command-line arguments. It takes two arguments: `argc` and `argv` from the `main()` function.


---

### Example Usage

#### Example 1: Simple Program with Arguments and Options

```cpp
#include <clicky/clicky.hpp>

int main(int argc, char* argv[]) {
    clicky cli; // no usage message 

    cli.add_option("option", "f", "This is an option", false);
    cli.add_argument("argument", "a", "This is an argument", true);

    cli.parse(argc, argv);
}
```

Usage with `g++`:
```bash
g++ -std=c++20 -o some_program some_program.cpp -lclicky
```

#### Example 2: Program with Usage Message, Arguments, and Options

```cpp
#include <clicky/clicky.hpp>

int main(int argc, char* argv[]) {
    clicky cli("{program} [OPTIONS]"); // usage message, using the {program} variable.
    
    cli.add_arguments({
                     {"argument", "a", "This is an argument", true},
                     {"argument2", "b", "This is another argument", false}
    });
    cli.parse(argc, argv);
}
```

Usage with `g++`:
```bash
g++ -std=c++20 -o some_other_program some_other_program.cpp -lclicky
```

Running the program:
```
./some_other_program --help
Usage: 
  ./some_other_program [OPTIONS]

Flags:
  --help, -h         : Display this help message (default: false)

Arguments:
  --argument2, -b    : This is another argument (optional)
  --argument, -a     : This is an argument (required)
```

---

### Contributing

If you think my code is shit, it's because it most likely is, and if you have any advice for me on how to improve my code, please reach out on discord <actually.auth>, or create an issue on this repo.

---
