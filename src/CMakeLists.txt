
add_library(simulator
            instruction.cpp
            machine.cpp
            source_parser.cpp
            simulator.cpp)

target_include_directories(simulator PUBLIC ../include)

target_compile_features(simulator PUBLIC cxx_std_11)

add_executable(cli_simulator
               cli.cpp)

target_link_libraries(cli_simulator
                      simulator)