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
  flex_2nd_is_register = false;
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

uint32_t Instruction::get_last_register() const {
  assert(!registers.empty());
  return registers[registers.size() - 1];
}
int32_t Instruction::get_second_operand() const { return flex_2nd_operand; }

void Instruction::set_second_operand(int64_t new_value) {
  flex_2nd_operand = new_value;
  flex_2nd_is_register = false;
}

void Instruction::set_is_2nd_operand_register(bool is_register) {
  flex_2nd_is_register = is_register;
}

bool Instruction::is_2nd_operand_register() const {
  return flex_2nd_is_register;
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

void Instruction::set_opcode(opcodes new_opcode) { opcode = new_opcode; }

void Instruction::set_condition_code(condition_codes new_condition_code) {
  condition_code = new_condition_code;
}

void Instruction::set_update_mode(update_modes new_update_mode) {
  update_mode = new_update_mode;
}

void Instruction::set_registers(std::vector<uint8_t> new_registers) {
  registers = new_registers;
}

size_t Instruction::get_register_count() const { return registers.size(); }
