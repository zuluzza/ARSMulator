
project(unittests LANGUAGES CXX)

add_executable(unittests 
			   test_machine.cpp
			   test_machine_byte.cpp)

target_include_directories(unittests PUBLIC ../include)

find_package(Catch2)

target_link_libraries(unittests
					  Catch2::Catch2WithMain
					  simulator)

add_test(test_all unittests)
