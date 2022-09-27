#ifndef MACHINE_H
#define MACHINE_H

#include "instruction.h"
#include "machine_byte.h"

#include <stdint.h>
#include <stdlib.h>
#include <vector>

#define SHIFT_CPRS_N 31
#define SHIFT_CPRS_Z 30
#define SHIFT_CPRS_C 29
#define SHIFT_CPRS_V 28
#define BITMASK_CPSR_N (0x01 << SHIFT_CPRS_N)
#define BITMASK_CPSR_Z (0x01 << SHIFT_CPRS_Z)
#define BITMASK_CPSR_C (0x01 << SHIFT_CPRS_C)
#define BITMASK_CPSR_V (0x01 << SHIFT_CPRS_V)

class Machine {
public:
  Machine(int mem_size);
  Machine(Machine &machine) = delete;
  ~Machine();
  void execute(Instruction i);
  void set_register_value(uint8_t reg_number, Machine_byte value);
  Machine_byte get_register_value(uint8_t reg_number);
  uint32_t get_current_program_status_register();
  void set_current_program_status_register(uint32_t register_value);
  void print_registers();
  bool meets_condition_code(condition_codes code);

private:
  void execute_add(Instruction i);
  void execute_subtract(Instruction i);

  std::vector<Machine_byte> registers;
  uint32_t current_program_status_register;
  char *memory;
};
#endif // MACHINE_H