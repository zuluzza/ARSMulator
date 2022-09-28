#include "instruction.h"
#include "machine.h"

#include <cassert>
#include <fstream>
#include <iostream>

int main(int argc, char *argv[]) {
  assert(argc == 2);

  std::ifstream asm_file_in;
  asm_file_in.open(argv[1]);

  std::string instruction_line;
  Machine machine(1024);

  while (asm_file_in) {
    getline(asm_file_in, instruction_line);
    std::cout << instruction_line << std::endl;
  }

  asm_file_in.close();

  return 0;
}