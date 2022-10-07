#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>

#include "simulator.h"

TEST_CASE("Simulator, run program to the end") {
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

  Simulator::run_program(program, m);
  CHECK(70 == m.get_register_value(1).to_unsigned32());
}

TEST_CASE("Simulator, run 2 instructions, stop, continue 1 more instruction") {
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
  program.push_back({opcodes::ADD,
                     condition_codes::NONE,
                     suffixes::NONE,
                     update_modes::NONE,
                     {0, 1},
                     150});
  Machine m(1024);
  Simulator::run_program(program, m, 2);
  CHECK(70 == m.get_register_value(1).to_unsigned32());
  Simulator::run_program(program, m, 2);
  CHECK(220 == m.get_register_value(0).to_unsigned32());
}
