project('clicky', 'cpp',
    version: '2.4',
    default_options: ['cpp_std=c++20', 'warning_level=3', 'buildtype=release'])

# Directories
src_dir = 'src'
include_dir = 'include'

# Compiler flags
cpp_args = ['-Wall', '-Wextra', '-Werror', '-pedantic']
include_dirs = include_directories(include_dir)

# Source files
sources = files(
  'src/clicky.cpp',
  'include/clicky.hpp'
)

# Static library
static_lib = static_library('clicky', 
    sources,
    include_directories: include_dirs,
    cpp_args: cpp_args,
    install: true
)

# Shared library
shared_lib = shared_library('clicky', 
    sources,
    include_directories: include_dirs,
    cpp_args: cpp_args,
    install: true
)

# Install headers
install_headers('include/clicky.hpp', subdir: 'clicky')

