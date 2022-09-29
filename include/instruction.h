#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <string>

// Opcodes for all architectures from
// https://developer.arm.com/documentation/dui0068/b/ARM-Instruction-Reference?lang=en
enum class opcodes {
  NONE = 0,
  ADC,
  ADD,
  AND,
  B,
  BIC,
  BL,
  CMN,
  CMP,
  EOR,
  LDM,
  LDR,
  MOV,
  MVN,
  ORR,
  RSB,
  RSC,
  SBC,
  STM,
  STR,
  SUB,
  SWI,
  TEQ,
  TST
};

enum class condition_codes {
  NONE = 0,
  AL,
  EQ,
  NE,
  CS,
  CC,
  MI,
  PL,
  VS,
  VC,
  HI,
  LS,
  GE,
  LT,
  GT,
  LE
};

enum class update_modes { NONE = 0, FA, EA, FD, ED };

enum class suffixes { NONE = 0, S, B, SH, H, SB, D };

class Instruction {
public:
  Instruction(opcodes operation, condition_codes condition, suffixes suf,
              update_modes update, uint32_t reg1, uint32_t reg2,
              int64_t second_operand);
  Instruction(const Instruction &i) = default;

  bool has_condition_code() const;
  bool has_updade_mode() const;

  opcodes get_opcode() const;
  condition_codes get_condition_code() const;
  uint32_t get_register_1() const;
  uint32_t get_register_2() const;
  void set_register_1(uint8_t new_value);
  int32_t get_second_operand() const;
  void set_second_operand(int64_t new_value);
  bool get_update_condition_flags() const;
  void set_suffix(suffixes suf);
  suffixes get_suffix() const;

private:
  opcodes opcode;
  condition_codes condition_code;
  suffixes suffix;
  update_modes update_mode;
  uint32_t register_1;
  uint32_t register_2;
  int64_t flex_2nd_operand;
};

#endif // INSTRUCTION_H