# ARSMulator

An assembly simulator that simulates ARM processor. The supported instruction set is not any specific ARM architecture, but includes instructions that are supported by all architectures in [ARM instruction reference](https://developer.arm.com/documentation/dui0068/b/ARM-Instruction-Reference?lang=en)

## Building

Requires CMake and a compatible c++ compiler (tested with clang).  Below commands also use Ninja build system, but it's not necessary, if you prefer to use something else
>cmake -G "Ninja" CMakeLists.txt

>cmake --build .

## Running

There's a simple command line tool that can be used to run simulations. After building it can be run by 
>./src/build/cli_simulator.exe [-m 256] [-f c\:/git/ARSMulator/test.s]

There are two command line options:
-m Sets the memory size of the simulated machine in bytes (default is 256)
-f Path to the source code file that is to be run

The are following commands that can be given to the command line simulator
h: display this help
r: run program until it's stopped
s: run one instruction
x{X}: run X instructions and stop
p: print register values
m{X}: print memory at address X
q: quit

## Unit test

Unit tests utilize [Catch2](https://github.com/catchorg/Catch2). Instructions to install catch2 can be found in it's [documentation](https://github.com/catchorg/Catch2/blob/devel/docs/cmake-integration.md#installing-catch2-from-git-repository)

After a succesful build, all unit tests can be run by
>ctest --output-on-failure

### TODO
1. add a parser to run .s files
2. add CLI
