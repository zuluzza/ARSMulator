#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "instruction.h"
#include "machine.h"

#include <string>
#include <vector>

class Simulator {
public:
  static void run_program(std::vector<Instruction> &program, Machine &m);
};

#endif // SIMULATOR_H