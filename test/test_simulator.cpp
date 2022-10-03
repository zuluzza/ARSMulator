#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>

#include "simulator.h"

TEST_CASE("Simulator, run_program") {
  std::vector<Instruction> program;
  program.push_back({opcodes::MOV,
                     condition_codes::NONE,
                     suffixes::NONE,
                     update_modes::NONE,
                     {2},
                     20});
  program.push_back({opcodes::ADD,
                     condition_codes::NONE,
                     suffixes::NONE,
                     update_modes::NONE,
                     {1, 2},
                     50});
  Machine m(1024);
  CHECK(70 == m.get_register_value(1));
}