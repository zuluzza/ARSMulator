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

enum class shift_ops { NONE = 0, LSL, LSR, ASR, ROR, RRX };

class Instruction {
public:
  Instruction(opcodes operation, condition_codes condition,
              bool update_condition_code_flags, update_modes update,
              shift_ops shift, uint32_t reg1, uint32_t reg2,
              int64_t second_operand);
  Instruction(const Instruction &i) = default;

  bool has_condition_code();
  bool has_updade_mode();
  bool has_shift_op();

  opcodes get_opcode();
  condition_codes get_condition_code();
  uint32_t get_register_1();
  uint32_t get_register_2();
  void set_register_1(uint8_t new_value);
  int32_t get_second_operand();
  bool get_update_condition_flags();
  void set_update_condition_flags(bool new_status);

private:
  opcodes opcode;
  condition_codes condition_code;
  bool update_flags;
  update_modes update_mode;
  shift_ops shift_op;
  uint32_t register_1;
  uint32_t register_2;
  int64_t flex_2nd_operand;
};

#endif // INSTRUCTION_H