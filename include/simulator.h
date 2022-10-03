#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "instruction.h"
#include "machine.h"

class Simulator() {
public:
  static void run_program(std::vector<Instruction> & program, Machine & m);
}

#endif // SIMULATOR_H