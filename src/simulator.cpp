#include "simulator.h"
#include "instruction.h"
#include "machine.h"

#include <iostream>
#include <vector>

void Simulator::run_program(std::vector<Instruction> &program, Machine &m,
                            unsigned int count) {
  bool cont = true;
  bool stop_after_count_instructions = (count != 0);
  while (cont) {
    unsigned int instruction_address =
        m.get_register_value(PROGRAM_COUNTER_INDEX).to_unsigned32();
    if (instruction_address >= program.size()) {
      break;
    }
    Instruction i = program[instruction_address];
    cont = !m.execute(i);
    if (stop_after_count_instructions) {
      count--;
      if (count == 0) {
        cont = false;
      }
    }
  }
  std::cout << "Program halted!" << std::endl;
}
