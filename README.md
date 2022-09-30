# ARSMulator

An assembly simulator that simulates ARM processor. The supported instruction set is not any specific ARM architecture, but includes instructions that are supported by all architectures in [ARM instruction reference](https://developer.arm.com/documentation/dui0068/b/ARM-Instruction-Reference?lang=en)

## Building

Requires CMake and a compatible c++ compiler (tested with clang).  Below commands also use Ninja build system, but it's not necessary, if you prefer to use something else
>cmake -G "Ninja" CMakeLists.txt

>cmake --build .

## Unit test

Unit tests utilize [Catch2](https://github.com/catchorg/Catch2). Instructions to install catch2 can be found in it's [documentation](https://github.com/catchorg/Catch2/blob/devel/docs/cmake-integration.md#installing-catch2-from-git-repository)

After a succesful build, all unit tests can be run by
>ctest --output-on-failure

### TODO
1. add a runner that runs through the program
2. add SWI (SW interrupt) instruction
3. add a parser to run .s files
