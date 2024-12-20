# Compiler and Flags
CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -pedantic -O2 -fPIC

# Directories
INCLUDEDIR := include
SRCDIR := src
BUILDDIR := build
PREFIX := /usr/local
INSTALL_INCLUDEDIR := $(PREFIX)/include
INSTALL_LIBDIR := $(PREFIX)/lib

# Target Names
STATIC_LIB := libclicky.a
DYNAMIC_LIB := libclicky.so
SOURCES := $(wildcard $(SRCDIR)/*.cpp)
HEADERS := $(wildcard $(INCLUDEDIR)/*.hpp)
OBJECTS := $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SOURCES))

.PHONY: all clean install uninstall

# Default Target: Build both static and dynamic libraries
all: $(STATIC_LIB) $(DYNAMIC_LIB)

# Build the static library
$(STATIC_LIB): $(OBJECTS)
	@echo "Creating static library: $@"
	ar rcs $@ $^

# Build the dynamic library
$(DYNAMIC_LIB): $(OBJECTS)
	@echo "Creating dynamic library: $@"
	$(CXX) -shared -o $@ $^

# Compile source files to object files
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS)
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -I$(INCLUDEDIR) -c $< -o $@

# Clean up build files
clean:
	@echo "Cleaning up..."
	rm -rf $(BUILDDIR) $(STATIC_LIB) $(DYNAMIC_LIB)

# Install the library and header
install: $(STATIC_LIB) $(DYNAMIC_LIB)
	@echo "Installing library and headers..."
	install -d $(INSTALL_INCLUDEDIR)
	install -d $(INSTALL_LIBDIR)
	install -m 644 $(INCLUDEDIR)/clicky.hpp $(INSTALL_INCLUDEDIR)
	install -m 644 $(STATIC_LIB) $(INSTALL_LIBDIR)
	install -m 755 $(DYNAMIC_LIB) $(INSTALL_LIBDIR)

# Uninstall the library and header
uninstall:
	@echo "Uninstalling library and headers..."
	rm -f $(INSTALL_INCLUDEDIR)/clicky.hpp
	rm -f $(INSTALL_LIBDIR)/$(STATIC_LIB)
	rm -f $(INSTALL_LIBDIR)/$(DYNAMIC_LIB)

