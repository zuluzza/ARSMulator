#include "machine.h"
#include "instruction.h"

#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>

#define REGISTER_COUNT 16

Machine::Machine(int mem_size) {
  memory = static_cast<char *>(malloc(mem_size * sizeof(char)));
  // Fail if it was not able to reserve memory
  assert(memory);

  registers = std::vector<Machine_byte>(REGISTER_COUNT, 0);
  current_program_status_register = 0;
}

Machine::~Machine() {
  if (memory) {
    free(memory);
  }
}

void Machine::execute(Instruction i) {
  switch (i.get_opcode()) {
  case opcodes::ADD:
    execute_add(i);
    break;
  case opcodes::SUB:
    execute_subtract(i);
    break;
  case opcodes::NONE:
    // TODO add a log?
  default:
    assert(false);
  }
}

void Machine::execute_add(Instruction i) {
  assert(i.get_opcode() == opcodes::ADD);

  // only executed if the condition code flags in the CPSR meet the specified
  // condition
  if (!meets_condition_code(i.get_condition_code())) {
    return;
  }

  const uint32_t register_to_write = i.get_register_1();
  assert(register_to_write < REGISTER_COUNT);

  Machine_byte operand_byte(i.get_second_operand());
  registers[register_to_write] = registers[i.get_register_2()] + operand_byte;

  if (i.get_update_condition_flags()) {
    const int64_t result =
        static_cast<int64_t>(registers[i.get_register_2()].to_signed32()) +
        static_cast<int64_t>(i.get_second_operand());
    current_program_status_register |= ((result < 0) << SHIFT_CPRS_N);
    current_program_status_register |= ((result == 0) << SHIFT_CPRS_Z);

    // For an addition C is set to 1 if the addition produced a carry (that is,
    // an unsigned overflow), and to 0 otherwise.
    current_program_status_register |=
        ((registers[i.get_register_2()].get_carry()) << SHIFT_CPRS_C);
    // Overflow occurs if the result of an add, subtract, or compare is greater
    // than or equal to 2^31, or less than -2^31
    current_program_status_register |=
        (((result >= std::pow(2, 31)) || (result < -std::pow(2, 31)))
         << SHIFT_CPRS_V);
  }
}

void Machine::execute_subtract(Instruction i) {
  assert(i.get_opcode() == opcodes::SUB);

  if (!meets_condition_code(i.get_condition_code())) {
    return;
  }

  const uint32_t register_to_write = i.get_register_1();
  assert(register_to_write < REGISTER_COUNT);

  Machine_byte operand_byte(i.get_second_operand());
  registers[register_to_write] = registers[i.get_register_2()] - operand_byte;

  if (i.get_update_condition_flags()) {
    const int64_t result =
        static_cast<int64_t>(registers[i.get_register_2()].to_signed32()) -
        static_cast<int64_t>(i.get_second_operand());
    current_program_status_register |= ((result < 0) << SHIFT_CPRS_N);
    current_program_status_register |=
        ((registers[i.get_register_2()].get_bits() == operand_byte.get_bits())
         << SHIFT_CPRS_Z);

    // A carry occurs if the result of a subtraction is positive
    current_program_status_register |=
        ((!registers[i.get_register_2()].get_borrow()) << SHIFT_CPRS_C);
    // Overflow occurs if the result of an add, subtract, or compare is greater
    // than or equal to 2^31, or less than -2^31
    current_program_status_register |=
        (((result >= std::pow(2, 31)) || (result < -std::pow(2, 31)))
         << SHIFT_CPRS_V);
  }
}

void Machine::set_register_value(uint8_t reg_number, Machine_byte value) {
  assert(reg_number < REGISTER_COUNT);
  registers[reg_number] = value;
}

Machine_byte Machine::get_register_value(uint8_t reg_number) {
  assert(reg_number < REGISTER_COUNT);
  return registers[reg_number];
}

void Machine::print_registers() {
  for (uint8_t i = 0; i < REGISTER_COUNT; ++i) {
    std::cout << "Register " << i << ": " << registers[i].get_bits()
              << std::endl;
  }
}

uint32_t Machine::get_current_program_status_register() {
  return current_program_status_register;
}

bool Machine::meets_condition_code(condition_codes code) {
  switch (code) {
  case condition_codes::EQ:
    return BITMASK_CPSR_Z & current_program_status_register;
  case condition_codes::NE:
    return !static_cast<bool>(BITMASK_CPSR_Z & current_program_status_register);
  case condition_codes::CS:
    return BITMASK_CPSR_C & current_program_status_register;
  case condition_codes::CC:
    return !static_cast<bool>(BITMASK_CPSR_C & current_program_status_register);
  case condition_codes::MI:
    return BITMASK_CPSR_N & current_program_status_register;
  case condition_codes::PL:
    return !static_cast<bool>(BITMASK_CPSR_N & current_program_status_register);
  case condition_codes::VS:
    return BITMASK_CPSR_V & current_program_status_register;
  case condition_codes::VC:
    return !static_cast<bool>(BITMASK_CPSR_V & current_program_status_register);
  case condition_codes::HI:
    return (BITMASK_CPSR_C & current_program_status_register) &&
           !static_cast<bool>(BITMASK_CPSR_Z & current_program_status_register);
  case condition_codes::LS:
    return !static_cast<bool>(BITMASK_CPSR_C &
                              current_program_status_register) ||
           (BITMASK_CPSR_Z & current_program_status_register);
  case condition_codes::GE:
    return static_cast<bool>(BITMASK_CPSR_N &
                             current_program_status_register) ==
           static_cast<bool>(BITMASK_CPSR_V & current_program_status_register);
  case condition_codes::LT:
    return static_cast<bool>(BITMASK_CPSR_N &
                             current_program_status_register) !=
           static_cast<bool>(BITMASK_CPSR_V & current_program_status_register);
  case condition_codes::GT:
    return !static_cast<bool>(BITMASK_CPSR_Z &
                              current_program_status_register) &&
           (static_cast<bool>(BITMASK_CPSR_N &
                              current_program_status_register) ==
            static_cast<bool>(BITMASK_CPSR_V &
                              current_program_status_register));
  case condition_codes::LE:
    return (BITMASK_CPSR_Z & current_program_status_register) &&
           (static_cast<bool>(BITMASK_CPSR_N &
                              current_program_status_register) !=
            static_cast<bool>(BITMASK_CPSR_V &
                              current_program_status_register));
  default:
    // TODO log missing code
  case condition_codes::AL:   // intentional fall-through
  case condition_codes::NONE: // intentional fall-through
    return true;
  }
}

void Machine::set_current_program_status_register(uint32_t register_value) {
  current_program_status_register = register_value;
}