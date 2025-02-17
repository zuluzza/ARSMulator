#ifndef MACHINE_H
#define MACHINE_H

#include "instruction.h"
#include "machine_byte.h"

#include <cstdint>
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
#define PROGRAM_COUNTER_INDEX 15
#define LINK_REGISTER_INDEX 14

class Machine {
public:
  Machine(int mem_size);
  Machine(Machine &machine) = delete;
  ~Machine();
  // returns true if machine should be halted (due to SWI or an error), false
  // otherwise
  bool execute(Instruction i);
  void set_register_value(uint8_t reg_number, Machine_byte value);
  Machine_byte get_register_value(uint8_t reg_number);
  uint32_t get_current_program_status_register();
  void set_current_program_status_register(uint32_t register_value);
  void print_registers();
  bool meets_condition_code(condition_codes code);
  void set_memory(int address, Machine_byte byte);
  Machine_byte get_memory(int address);
  Machine_byte get_flex_2nd_operand_value(Instruction i);

private:
  void execute_add(Instruction i, bool use_carry = false);
  void execute_subtract(Instruction i, bool use_carry = false);
  void execute_and(Instruction i);
  void execute_eor(Instruction i);
  void execute_orr(Instruction i);
  void execute_load(Instruction i);
  void execute_store(Instruction i);
  void execute_move(Instruction i);
  void execute_load_multiple(Instruction i);
  void execute_store_multiple(Instruction i);

  std::vector<Machine_byte> registers;
  uint32_t current_program_status_register;
  Machine_byte *memory;
  int memory_size;
};
#endif // MACHINE_H