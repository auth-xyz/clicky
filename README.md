-----
### CLIcky

**clicky** is a C++ library that I've used in past projects, but never really published it as its own thing. In simple terms, it's a simplified `argparse` from python written in C++.

-----

- `What is clicky`:
    `clicky` As the introduction explains, it's a C++ library that handles arguments, flags and parses them from the argc and argv.

- `How is it useful?`:
	It facilitates the creation of CLI tools that requires the user to input multiple arguments or flags.

- `Why?`:
    why not?

----

### Setting up

So, setting up clicky is pretty easy, I made sure that the process is pretty straight forward, as to keep its simplicity.
However, you will need to instal `meson` and `ninja` if you want to compile it by yourself.

```bash
$ git clone https://github.com/auth-xyz/clicky
$ cd clicky/
$ meson setup build
$ ninja -C build
$ sudo ninja install -C build
```

One thing worth mentioning here:
If you are installing the library, make sure to link it when compiling using the `-lclicky` flag.
`(ex: g++ somefile.cpp -lclicky)`

----

### Documentation

So, `clicky`, though pretty straight forward, needs a bit of explaining. Everything is meant to be very simple to use, if you have any problems using it, you can check the `examples/` folder. But here's a simple-ish explanation of its core funcionality:

```cpp
#include <clicky/clicky.hpp>

int main(int argc, char* argv[]) {
	clicky cli; 

	cli.add_argument("", "", true, "");
	// --------------^ 
	// ------------------^
	// ----------------------^
	// ----------------------------^
}
```

There's only a few functions within clicky to mess around with, `add_argument(name, alias, required, description)` has 4 parameters (as shown above). The other function you might use aswell is the `add_flag(name, alias, default, description)`, unlike `add_argument()`, `add_flag()` has the `default` property, (or in the code `default_value`), which can be either set to true or false. 
The third function, and the one that actually makes everything work, is `parse(argc, argv)`, it needs no explanation.

Other notable mentions are `flag(flag_name)` and `argument(argument_name)`, both of them return the argument's value so you can do some logic work there.

Now I know the documentation is lacking a bit, and I want to improve it, but hey, that's a problem for future me! You can check the examples for a better explanation.

----

### Contributing

If you think my code is shit, it's because it most likely is, and if you have any advice for me on how to improve my code, please reach out on discord <actually.auth>, or create an issue on this repo.


[logo]: https://github.com/auth-xyz/assets/blob/main/logos/chunky.png?raw=true

