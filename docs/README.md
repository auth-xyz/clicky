<p align="center">
    <img src="https://github.com/auth-xyz/assets/blob/main/logos/chunky.png?raw=true" alt="logo" width="400" height="400">
</p>

### CLIcky

**clicky** is a simple C++ library for handling command-line arguments and flags. It allows you to define flags and arguments, parse input, and display formatted help messages. You can easily add required or optional arguments, set default values for flags, and access both positional and named arguments. The library supports aliases for flags and arguments, ensures all required arguments are provided, and presents clear, color-coded help information for better usability.

----

Now, you might be asking me: "Auth, how do I install this marvelous creation?", and I am here to answer that question my friend!
You see, installing `clicky` will require you to do a bit of setting up, nothing out of the ordinary, just installing a package or two.
Below, is how you install the two *main* requirements to build `clicky`. Now, of course, if you don't want to install clicky, you can 
check the [releases](https://github.com/auth-xyz/clicky/releases) page!

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
Now I need to put this reminder here, whenever you use clicky, you *need* to pass the -lclicky flag to `g++`
*only if you're installed the library*

----

### Documentation

So, I'll try to make this documentation concise and easy to digest.
Below, you'll find a list of the methods that are under the public interface of `clicky`:

`clicky::clicky()` - `constructor`
- Takes a single (optional) `std::string` argument that is the usage of the program. The usage can have a builtin variable `{program}` which will be replaced with the argv[0].


`clicky::add_argument(const std::string& name, const std::string& alias, bool required, const std::string& description)` - `method` 
- Takes 4 arguments, the name of the argument, its alias, if its required or not, and the description of the argument. The `clicky::add_arguments()` method is a convenience method that allows you to add multiple arguments at once.


`clicky::add_flag(const std::string& name, const std::string& alias, bool required, const std::string& description)` - `method` 
- Takes 4 arguments, the name of the flag, its alias, its default value (either true or false), and the description of the flag. The `clicky::add_flags()` method is a convenience method that allows you to add multiple flags at once.

`clicky::set_prefix(const std::vector<std::string>& arg_prefixes, const std::vector<std::string>& flag_prefixes)` - `method`
- Takes 2 arguments, the prefixes for arguments and flags. See `examples/02.cpp` for a better understanding

`clicky::parse(int argc, char* argv[])` - `method`
- Takes 2 arguments, the argc and argv of the program. Does all the heavy lifting.



An example of how you'd use clicky in action:
```cpp
#include <clicky/clicky.hpp>

int main(int argc, char* argv[]) {
   clicky cli; // no usage message 

   cli.add_flag("flag", "f", false, "This is a flag");
   cli.add_argument("argument", "a", true, "This is an argument");

   cli.parse(argc, argv);
}
```
`g++ -std=c++20 -o some_program some_program.cpp -lclicky`

```cpp
#include <clicky/clicky.hpp>

int main(int argc, char* argv[]) {
   clicky cli("{program} [OPTIONS]"); // usage message, using the {program} variable.
   
   cli.add_arguments({
                     {"argument", "a", true, "This is an argument"},
                     {"argument2", "b", false, "This is another argument"}
   });
   cli.parse(argc,argv)
}
```

`g++ -std=c++20 -o some_other_program some_other_program.cpp -lclicky`

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

That's *pretty much* all that clicky has (for now), I plan to expand the project further, add more things and maybe, just maybe, put it in the official dnf repository, but that's for the future.
There are some other methods that I haven't mentioned, but they are self-explanatory.

----

### Contributing

If you think my code is shit, it's because it most likely is, and if you have any advice for me on how to improve my code, please reach out on discord <actually.auth>, or create an issue on this repo.


