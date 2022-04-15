# ImmortalThreads runtime

The ImmortalThreads runtime is a static library that should be linked to the application.

## Add this library to a project using CMake

The application developer can include the ImmortalThreads runtime via CMake simply by setting some CMake variables and adding the this directory using `add_subdirectory`. The target `immortality::immortality` should be linked to the application executable.

The accepted parameters are:

* `IMMORTALITY_PORT` (string, mandatory): specifies the port to be used. The supported values are:
  * `linux`
  * `msp430`

  For a comprehensive list of supported ports, please look in the `./port/` folder.

  The application developer needs to ensure that the correct CMake toolchain file is used, when the selected port target is an embedded target (i.e. not Linux).
  Each port may provide a linker script named `./port/<port>/linker.ld`. The application developer needs to use the provided linker script instead of the default one.

Example application `CMakeLists.txt`.

```cmake
# When building the runtime for the host platform (i.e. linux)
project(app)
add_executable(app main.c)
set(IMMORTALITY_PORT "linux")
add_subdirectory(${PATH_TO_THIS_DIRECTORY})
target_link_libraries(app
  PRIVATE
  immortality::immortality
)

# When building the runtime for msp430
set(CMAKE_TOOLCHAIN_FILE ${PATH_TO_THIS_DIRECTORY}/cmake/toolchains/msp430fr5994.cmake)
project(app)
add_executable(app main.c)
set(IMMORTALITY_PORT "msp430")
add_subdirectory(${PATH_TO_THIS_DIRECTORY})
target_link_libraries(app
  PRIVATE
  immortality::immortality
)
target_link_options(app
  PRIVATE
  # Specify custom linker script
  -T${PATH_TO_THIS_DIRECTORY}/src/port/msp430/linker.ld
  )
```

## Automatic source transformation using CMake

So far we have seen how to add the ImmortalThreads runtime as a static library in our project, but we assumed that all the source files were already transformed. In order to automatically transform the original source files during the build, the utility functions provided in [ImmortalCompiler.cmake](./cmake/ImmortalCompiler.cmake) can be used.

Executable targets and static library targets in CMake can be automatically instrumented by passing them to the respective `add_immortal_library` and `add_immortal_executable` functions. A new target with name the original target's name appended "_immortal" will be created and the original target will be excluded from the default compilation.

```cmake
add_executable(app main.c)
include(${PATH_TO_THIS_DIRECTORY}/cmake/ImmortalCompiler.cmake)
# The target app_immortal will be created. When building the app_immortal
# target, the # source files will be automatically transformed and built.
add_immortal_executable(app)
```

## Build

This folder comes with a set of test executables with are built if CMake is directly used on this folder.
They can be built running the following commands:

```sh
cmake -B<build directory> -H. -DIMMORTALITY_PORT="<port>"
cmake --build <build directory>
```

The built executables can be found in `<build directory>/bin`
The built ImmortalThreads static library can be found in `<build directory>/lib`
