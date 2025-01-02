<p>
    <img src="https://github.com/auth-xyz/assets/blob/main/logos/chunky.png?raw=true" alt="logo" width="100" height="100">
</p>

### CLIcky

**clicky**, a C++ library meant to facilitate the creation of command line interfaces for apps!
(only for Linux (for now, I promise......(i think)))

----

### Setting up:
#### Installation
##### Requirements:
- [meson](https://mesonbuild.com/)
- [ninja](https://ninja-build.org/)

##### Installation:
```bash
git clone https://github.com/auth-xyz/clicky
cd clicky/

meson setup build
ninja -C build
sudo ninja install -C build
```

One thing worth mentioning here:
If you are installing the library, make sure to link it when compiling using the `-lclicky` flag.
`(ex: g++ somefile.cpp -lclicky)`

### Documentation

You can find the documentation [here](https://github.com/auth-xyz/clicky/blob/main/docs/README.md)

----

### Contributing

If you think my code is shit, it's because it most likely is, and if you have any advice for me on how to improve my code, please reach out on discord <actually.auth>, or create an issue on this repo.

