#include "machine.h"
#include "instruction.h"

#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>

#define REGISTER_COUNT 16
#define PROGRAM_COUNTER_INDEX 15
#define LINK_REGISTER_INDEX 14

Machine::Machine(int mem_size) {
  memory = static_cast<Machine_byte *>(malloc(mem_size * sizeof(Machine_byte)));
  // Fail if it was not able to reserve memory
  assert(memory);
  memory_size = mem_size;
  for (int i = 0; i < memory_size; ++i) {
    memory[i] = Machine_byte(0);
  }
  registers = std::vector<Machine_byte>(REGISTER_COUNT, 0);
  current_program_status_register = 0;
}

Machine::~Machine() {
  if (memory) {
    free(memory);
  }
}

void Machine::execute(Instruction i) {
  // only executed if the condition code flags in the CPSR meet the specified
  // condition
  if (!meets_condition_code(i.get_condition_code())) {
    return;
  }

  switch (i.get_opcode()) {
  case opcodes::ADC:
    execute_add(i, true);
    break;
  case opcodes::CMN: // Same as ADD except result is discarded
    i.set_suffix(suffixes::S);
    i.set_register(0, REGISTER_COUNT); // discards result
  case opcodes::ADD:                   // intentional fall-through
    execute_add(i, false);
    break;
  case opcodes::BIC:
    i.set_second_operand(~i.get_second_operand());
    execute_and(i);
    break;
  case opcodes::TST: // Performs bitwise but discards result
    i.set_suffix(suffixes::S);
    i.set_register(0, REGISTER_COUNT);
  case opcodes::AND: // intentional fall-through
    execute_and(i);
    break;
  case opcodes::TEQ: // Performs exclusive or but discards result
    i.set_suffix(suffixes::S);
    i.set_register(0, REGISTER_COUNT);
  case opcodes::EOR:
    execute_eor(i);
    break;
  case opcodes::ORR:
    execute_orr(i);
    break;
  case opcodes::CMP: // Same as SUB except result is discarded
    i.set_suffix(suffixes::S);
    i.set_register(0, REGISTER_COUNT); // discards result
  case opcodes::RSB:                   // intentional fall-through
  case opcodes::RSC:                   // intentional fall-through
  case opcodes::SBC:                   // intentional fall-through
  case opcodes::SUB:                   // intentional fall-through
    execute_subtract(i, ((i.get_opcode() == opcodes::SBC) ||
                         (i.get_opcode() == opcodes::RSC)));
    break;
  case opcodes::LDR:
    execute_load(i);
    break;
  case opcodes::STR:
    execute_store(i);
    break;
  case opcodes::MVN:
    i.set_second_operand(~i.get_second_operand());
  case opcodes::MOV: // intentional fall-through
    execute_move(i);
    break;
  case opcodes::BL:
    registers[LINK_REGISTER_INDEX] =
        Machine_byte(registers[PROGRAM_COUNTER_INDEX].to_unsigned32() + 1);
  case opcodes::B: // intentional fall-through
    registers[PROGRAM_COUNTER_INDEX] = i.get_second_operand();
    break;
  case opcodes::NONE:
    std::cout << "Instruction with opcode NONE" << std::endl;
  default:
    std::cout << "Unknown opcode " << static_cast<uint8_t>(i.get_opcode())
              << std::endl;
    assert(false);
  }
}

void Machine::execute_add(Instruction i, bool use_carry) {
  // calculate result of add operation
  Machine_byte operand_byte(i.get_second_operand());
  registers[i.get_register(1)].set_carry(use_carry);
  Machine_byte result_byte = registers[i.get_register(1)] + operand_byte;

  // update flags if requested
  if (i.get_update_condition_flags()) {
    const int64_t result =
        static_cast<int64_t>(registers[i.get_register(1)].to_signed32()) +
        static_cast<int64_t>(i.get_second_operand());
    current_program_status_register |= ((result < 0) << SHIFT_CPRS_N);
    current_program_status_register |= ((result == 0) << SHIFT_CPRS_Z);

    // For an addition C is set to 1 if the addition produced a carry (that is,
    // an unsigned overflow), and to 0 otherwise.
    current_program_status_register |=
        ((registers[i.get_register(1)].get_carry()) << SHIFT_CPRS_C);
    // Overflow occurs if the result of an add, subtract, or compare is greater
    // than or equal to 2^31, or less than -2^31
    current_program_status_register |=
        (((result >= std::pow(2, 31)) || (result < -std::pow(2, 31)))
         << SHIFT_CPRS_V);
  }

  // write result to register (it's not written for compare operation)
  const uint32_t register_to_write = i.get_register(0);
  if (register_to_write < REGISTER_COUNT) {
    registers[register_to_write] = result_byte;
  }
}

void Machine::execute_subtract(Instruction i, bool use_carry) {
  // calculate result of subtract operation
  Machine_byte operand_byte(i.get_second_operand());
  const Machine_byte carry_byte(use_carry ? 1 : 0);
  Machine_byte result_byte(std::bitset<BIT_COUNT>(0));
  if (opcodes::RSB == i.get_opcode() || opcodes::RSC == i.get_opcode()) {
    result_byte = operand_byte - registers[i.get_register(1)] - carry_byte;
  } else {
    result_byte = registers[i.get_register(1)] - operand_byte - carry_byte;
  }

  // update flags if requested
  if (i.get_update_condition_flags()) {
    const int64_t result =
        static_cast<int64_t>(registers[i.get_register(1)].to_signed32()) -
        static_cast<int64_t>(i.get_second_operand()) - carry_byte.to_signed32();
    current_program_status_register |= ((result < 0) << SHIFT_CPRS_N);
    current_program_status_register |=
        ((registers[i.get_register(1)].get_bits() == operand_byte.get_bits())
         << SHIFT_CPRS_Z);

    // A carry occurs if the result of a subtraction is positive
    current_program_status_register |=
        ((!registers[i.get_register(1)].get_borrow()) << SHIFT_CPRS_C);
    // Overflow occurs if the result of an add, subtract, or compare is greater
    // than or equal to 2^31, or less than -2^31
    current_program_status_register |=
        (((result >= std::pow(2, 31)) || (result < -std::pow(2, 31)))
         << SHIFT_CPRS_V);
  }

  // write result to register (it's not written for compare operation)
  const uint32_t register_to_write = i.get_register(0);
  if (register_to_write < REGISTER_COUNT) {
    registers[register_to_write] = result_byte;
  }
}

void Machine::execute_and(Instruction i) {
  // Execute bitwise and
  Machine_byte result_byte =
      registers[i.get_register(1)] & Machine_byte(i.get_second_operand());

  // update flags if requested
  if (i.get_update_condition_flags()) {
    current_program_status_register |=
        ((result_byte.to_signed32() < 0) << SHIFT_CPRS_N);
    current_program_status_register |=
        ((result_byte.to_unsigned32() == 0) << SHIFT_CPRS_Z);
  }

  // write result to register (it's not written for compare operation)
  const uint32_t register_to_write = i.get_register(0);
  if (register_to_write < REGISTER_COUNT) {
    registers[register_to_write] = result_byte;
  }
}

void Machine::execute_orr(Instruction i) {
  const uint32_t register_to_write = i.get_register(0);
  assert(register_to_write < REGISTER_COUNT);

  registers[register_to_write] =
      registers[i.get_register(1)] | Machine_byte(i.get_second_operand());

  // update flags if requested
  if (i.get_update_condition_flags()) {
    current_program_status_register |=
        ((registers[register_to_write].to_signed32() < 0) << SHIFT_CPRS_N);
    current_program_status_register |=
        ((registers[register_to_write].to_unsigned32() == 0) << SHIFT_CPRS_Z);
  }
}

void Machine::execute_eor(Instruction i) {
  // Execute exclusive or
  Machine_byte result_byte =
      registers[i.get_register(1)] ^ Machine_byte(i.get_second_operand());

  // update flags if requested
  if (i.get_update_condition_flags()) {
    current_program_status_register |=
        ((result_byte.to_signed32() < 0) << SHIFT_CPRS_N);
    current_program_status_register |=
        ((result_byte.to_unsigned32() == 0) << SHIFT_CPRS_Z);
  }

  // write result to register (it's not written for compare operation)
  const uint32_t register_to_write = i.get_register(0);
  if (register_to_write < REGISTER_COUNT) {
    registers[register_to_write] = result_byte;
  }
}

void Machine::execute_load(Instruction i) {
  assert(i.get_register(1) < memory_size);
  assert(i.get_register(0) < REGISTER_COUNT);

  switch (i.get_suffix()) {
  case suffixes::H:
  case suffixes::SH:
    registers[i.get_register(0)] =
        memory[registers[i.get_register(1)].to_unsigned32()] & 0xFFFF;
    break;
  case suffixes::B:
  case suffixes::SB: // intentional fall-through
    registers[i.get_register(0)] =
        memory[registers[i.get_register(1)].to_unsigned32()] & 0xFF;
    break;
  case suffixes::D:
    assert(i.get_register(1) + 1 < memory_size);
    assert(i.get_register(0) + 1 < REGISTER_COUNT);
    assert(i.get_register(0) % 2 == 0);
    assert(i.get_register(0) + 1 != i.get_register(1));
    registers[i.get_register(0) + 1] =
        memory[registers[i.get_register(1)].to_unsigned32() + 1];
  case suffixes::NONE: // intentional fall-through
  default:
    registers[i.get_register(0)] =
        memory[registers[i.get_register(1)].to_unsigned32()];
  }
}

void Machine::execute_store(Instruction i) {
  assert(i.get_register(1) < memory_size);
  assert(i.get_register(0) < REGISTER_COUNT);

  switch (i.get_suffix()) {
  case suffixes::H:
  case suffixes::SH:
    memory[registers[i.get_register(1)].to_unsigned32()] =
        registers[i.get_register(0)] & 0xFFFF;
    break;
  case suffixes::B:
  case suffixes::SB: // intentional fall-through
    memory[registers[i.get_register(1)].to_unsigned32()] =
        registers[i.get_register(0)] & 0xFF;
    break;
  case suffixes::D:
    assert(i.get_register(1) + 1 < memory_size);
    assert(i.get_register(0) + 1 < REGISTER_COUNT);
    assert(i.get_register(0) % 2 == 0);
    assert(i.get_register(0) + 1 != i.get_register(1));
    memory[registers[i.get_register(1)].to_unsigned32() + 1] =
        registers[i.get_register(0) + 1];
  case suffixes::NONE: // intentional fall-through
  default:
    memory[registers[i.get_register(1)].to_unsigned32()] =
        registers[i.get_register(0)];
  }
}

void Machine::execute_move(Instruction i) {
  assert(i.get_register(0) < REGISTER_COUNT);

  registers[i.get_register(0)] = Machine_byte(i.get_second_operand());

  if (i.get_update_condition_flags()) {
    current_program_status_register |=
        ((registers[i.get_register(0)].to_signed32() < 0) << SHIFT_CPRS_N);
    current_program_status_register |=
        ((registers[i.get_register(0)].to_unsigned32() == 0) << SHIFT_CPRS_Z);
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

void Machine::set_memory(int address, Machine_byte byte) {
  assert(address < memory_size);
  memory[address] = byte;
}

Machine_byte Machine::get_memory(int address) {
  assert(address < memory_size);
  return memory[address];
}
