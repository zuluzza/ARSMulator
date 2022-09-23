
add_library(simulator
            instruction.cpp
            machine.cpp
            simulator.cpp)

target_include_directories(simulator PUBLIC ../include)

target_compile_features(simulator PUBLIC cxx_std_11)
