#include "instruction.h"
#include <cassert>

Instruction::Instruction(opcodes operation, condition_codes condition,
                         suffixes suf, update_modes update,
                         std::vector<uint8_t> regs, int64_t second_operand) {
  opcode = operation;
  condition_code = condition;
  suffix = suf;
  update_mode = update;
  registers = regs;
  flex_2nd_operand = second_operand;
}

opcodes Instruction::get_opcode() const { return opcode; }

bool Instruction::has_condition_code() const {
  return condition_code == condition_codes::NONE;
}

uint32_t Instruction::get_register(uint8_t index) const {
  assert(index < registers.size());
  return registers[index];
}

void Instruction::set_register(uint8_t index, uint8_t new_value) {
  assert(index < registers.size());
  registers[index] = new_value;
}

int32_t Instruction::get_second_operand() const { return flex_2nd_operand; }

void Instruction::set_second_operand(int64_t new_value) {
  flex_2nd_operand = new_value;
}

bool Instruction::get_update_condition_flags() const {
  return suffix == suffixes::S;
}

void Instruction::set_suffix(suffixes suf) { suffix = suf; }

suffixes Instruction::get_suffix() const { return suffix; }

condition_codes Instruction::get_condition_code() const {
  return condition_code;
}

uint8_t Instruction::register_count() { return registers.size(); }

update_modes Instruction::get_update_mode() const { return update_mode; }
