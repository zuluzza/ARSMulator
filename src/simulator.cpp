#include "simulator.h"
#include "instruction.h"
#include "machine.h"

#include <vector>

static void Simulator::run_program(std::vector<Instruction> &program,
                                   Machine &m) {
  bool cont = true;
  while (cont) {
    unsigned int instruction_address =
        m.get_register_value(PROGRAM_COUNTER_INDEX);
    Instruction i program[instruction_address];
    cont = !m.execute(i);
  }
  std::cout << "Program halted!" << std::cout;
}

int main(int argc, char *argv[]) {
  std::vector<Instruction> program;
  Machine m(2048);
  run_program(program, m);
  return 0;
}