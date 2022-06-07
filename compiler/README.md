# Compiler instrumentation

The compiler instrumentation tool is called `immortalc`.

## Usage

### From CMake

The easiest way to use `immortalc` is via the utility functions provided in [ImmortalCompiler.cmake](../runtime/cmake/ImmortalCompiler.cmake). See [runtime docs](../runtime/README.md#automatic-source-transformation-using-cmake).

### CLI

`immortalc` can be used similarly to other famous Clang-based tools such as [clang-tidy](https://clang.llvm.org/extra/clang-tidy/).

To transform a source file, simply

```sh
$ immortalc test.c
```

By default, if any transformation has been performed, the file `test.immortal.c` will be created along `test.c`.

`immortalc` is a LibTooling-based tool, and it’s easier to work with if you set up a compile command database (`compile_commands.json`) for your project. Using the compile command database, `immortalc` can know all the include paths, macro definitions that should be used when analyzing and transforming the source file. CMake for example can be configured to generate such compile command database. By default `immortalc` searches for `compile_commands.json` through all parent paths of the first input file. You can also pass the `-b <build-path>` option to specify where to look for `compile_commands.json`.

You can also specify compilation options on the command line after `--`:

```sh
$ immortalc test.c -- -DMY_DEFINES
```

This also causes any `compile_commands.json` to be ignored. If you want to add extra compiler flags on top of the existing flags specified in `compile_commands.json`, use `--extra-arg` or `--extra-arg-before`:

```sh
$ immortalc test.c --extra-arg=-DMY_DEFINES --extra-arg=-Imy_project/include
```

For more information, refer to `immortalc --help`.

## Limitations

While `immortalc` (or any other Clang-based tool) makes the best effort to interpret the compiler flags specified in `compile_commands.json`, often there are flags whose "side effect" cannot be reproduced by `immortalc`.

For example, if the compiler that is used in `compile_commands.json` is `msp430-elf-gcc`, you must know that `msp430-elf-gcc` defines the macro `__MSP430__` and also defines some macros based on the value of the option `-mmcu=<value>` (e.g. if `-mmcu=msp430fr5994`, then `-D__MSP430FR5994__` is defined).
`immortalc` is not able to know from the `compile_commands.json` that it must define these macros when analyzing the source files that are being cross-compiled. So we need to tell `immortalc` to define these macros when instrumenting the cross-compiled source files, via `--extra-arg`. E.g.`immortalc ./main.c --extra-arg=-D__MSP430__ --extra-arg=-D__MSP430FR5994__`.

## Build

```sh
# Configure CMake
cmake -H. -Bbuild
# Alternatively, if you want to install it in a different place (e.g. ~/.local/)
cmake -H. -Bbuild -DCMAKE_INSTALL_PREFIX="~/.local/"

# build
make -C build
# install
make -C build install
```

## Test

```sh
# (Optional virtual environment)
# Create a virtual environment
$ python3 -m venv venv
# Enable the virtual environment
$ . venv/bin/activate
# Install all the required packages
$ pip3 install -r requirements.txt
# Configure CMake
cmake -H. -Bbuild
# build
cd build/
make
# test
ctest -V
# if you need to update the snapshots
make e2e-test-update-snapshots
```

## Credits

* [TICS](https://github.com/TUDSSL/TICS/blob/master/tics/source-instrumentation/memory-log-instrumentation/memlog.cpp)
* [Clang-Tidy](https://github.com/llvm/llvm-project/tree/main/clang-tools-extra/clang-tidy)
