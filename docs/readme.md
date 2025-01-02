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



