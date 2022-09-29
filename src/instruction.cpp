#include "instruction.h"

Instruction::Instruction(opcodes operation, condition_codes condition,
                         suffixes suf, update_modes update, uint32_t reg1,
                         uint32_t reg2, int64_t second_operand) {
  opcode = operation;
  condition_code = condition;
  suffix = suf;
  update_mode = update;
  register_1 = reg1;
  register_2 = reg2;
  flex_2nd_operand = second_operand;
}

opcodes Instruction::get_opcode() const { return opcode; }

bool Instruction::has_condition_code() const {
  return condition_code == condition_codes::NONE;
}

bool Instruction::has_updade_mode() const {
  return update_mode == update_modes::NONE;
}

uint32_t Instruction::get_register_1() const { return register_1; }
uint32_t Instruction::get_register_2() const { return register_2; }

void Instruction::set_register_1(uint8_t new_value) { register_1 = new_value; }

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
