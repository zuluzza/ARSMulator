#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>

#include "instruction.h"
#include "machine.h"

#include <cmath>

TEST_CASE("Machine, meets_condition_code->true") {
  Machine m(256);

  CHECK(m.meets_condition_code(condition_codes::NONE));
  CHECK(m.meets_condition_code(condition_codes::AL));
  CHECK(m.meets_condition_code(condition_codes::NE));
  CHECK(m.meets_condition_code(condition_codes::CC));
  CHECK(m.meets_condition_code(condition_codes::PL));
  CHECK(m.meets_condition_code(condition_codes::VC));

  m.set_current_program_status_register(BITMASK_CPSR_N | BITMASK_CPSR_Z |
                                        BITMASK_CPSR_C | BITMASK_CPSR_V);
  CHECK(m.meets_condition_code(condition_codes::EQ));
  CHECK(m.meets_condition_code(condition_codes::CS));
  CHECK(m.meets_condition_code(condition_codes::MI));
  CHECK(m.meets_condition_code(condition_codes::VS));

  // C set and Z clear
  m.set_current_program_status_register(BITMASK_CPSR_C);
  CHECK(m.meets_condition_code(condition_codes::HI));
  // C clear or Z set
  m.set_current_program_status_register(0x00);
  CHECK(m.meets_condition_code(condition_codes::LS));
  m.set_current_program_status_register(BITMASK_CPSR_Z);
  CHECK(m.meets_condition_code(condition_codes::LS));
  // N and V the same
  m.set_current_program_status_register(BITMASK_CPSR_N | BITMASK_CPSR_V);
  CHECK(m.meets_condition_code(condition_codes::GE));
  m.set_current_program_status_register(0x00);
  CHECK(m.meets_condition_code(condition_codes::GE));
  // N and V different
  m.set_current_program_status_register(BITMASK_CPSR_N);
  CHECK(m.meets_condition_code(condition_codes::LT));
  m.set_current_program_status_register(BITMASK_CPSR_V);
  CHECK(m.meets_condition_code(condition_codes::LT));

  // Z clear, and N and V the same
  m.set_current_program_status_register(BITMASK_CPSR_N | BITMASK_CPSR_V);
  CHECK(m.meets_condition_code(condition_codes::GT));
  m.set_current_program_status_register(0x00);
  CHECK(m.meets_condition_code(condition_codes::GT));
  // Z set, or N and V different
  m.set_current_program_status_register(BITMASK_CPSR_Z | BITMASK_CPSR_N);
  CHECK(m.meets_condition_code(condition_codes::LE));
  m.set_current_program_status_register(BITMASK_CPSR_Z | BITMASK_CPSR_V);
  CHECK(m.meets_condition_code(condition_codes::LE));
}

TEST_CASE("Machine, meets_condition_code->false") {
  Machine m(256);

  CHECK(false == m.meets_condition_code(condition_codes::EQ));
  CHECK(false == m.meets_condition_code(condition_codes::CS));
  CHECK(false == m.meets_condition_code(condition_codes::MI));
  CHECK(false == m.meets_condition_code(condition_codes::VS));

  m.set_current_program_status_register(BITMASK_CPSR_N | BITMASK_CPSR_Z |
                                        BITMASK_CPSR_C | BITMASK_CPSR_V);
  CHECK(false == m.meets_condition_code(condition_codes::NE));
  CHECK(false == m.meets_condition_code(condition_codes::CC));
  CHECK(false == m.meets_condition_code(condition_codes::PL));
  CHECK(false == m.meets_condition_code(condition_codes::VC));

  // C set and Z clear
  m.set_current_program_status_register(BITMASK_CPSR_C | BITMASK_CPSR_Z);
  CHECK(false == m.meets_condition_code(condition_codes::HI));
  m.set_current_program_status_register(0x00);
  CHECK(false == m.meets_condition_code(condition_codes::HI));
  // C clear or Z set
  m.set_current_program_status_register(BITMASK_CPSR_C);
  CHECK(false == m.meets_condition_code(condition_codes::LS));
  // N and V the same
  m.set_current_program_status_register(BITMASK_CPSR_N);
  CHECK(false == m.meets_condition_code(condition_codes::GE));
  m.set_current_program_status_register(BITMASK_CPSR_V);
  CHECK(false == m.meets_condition_code(condition_codes::GE));
  // N and V different
  m.set_current_program_status_register(BITMASK_CPSR_N | BITMASK_CPSR_V);
  CHECK(false == m.meets_condition_code(condition_codes::LT));
  m.set_current_program_status_register(0x00);
  CHECK(false == m.meets_condition_code(condition_codes::LT));

  // Z clear, and N and V the same
  m.set_current_program_status_register(BITMASK_CPSR_Z | BITMASK_CPSR_N |
                                        BITMASK_CPSR_V);
  CHECK(false == m.meets_condition_code(condition_codes::GT));
  m.set_current_program_status_register(BITMASK_CPSR_N);
  CHECK(false == m.meets_condition_code(condition_codes::GT));
  m.set_current_program_status_register(BITMASK_CPSR_V);
  CHECK(false == m.meets_condition_code(condition_codes::GT));
  // Z set, or N and V different
  m.set_current_program_status_register(BITMASK_CPSR_Z | BITMASK_CPSR_N |
                                        BITMASK_CPSR_V);
  CHECK(false == m.meets_condition_code(condition_codes::LE));
  m.set_current_program_status_register(BITMASK_CPSR_N);
  CHECK(false == m.meets_condition_code(condition_codes::LE));
  m.set_current_program_status_register(BITMASK_CPSR_V);
  CHECK(false == m.meets_condition_code(condition_codes::LE));
}

TEST_CASE("Machine, execute ADD") {
  Machine m(1024);
  m.set_register_value(1, Machine_byte(10));
  Instruction i(opcodes::ADD, condition_codes::NONE, false, update_modes::NONE,
                shift_ops::NONE, 0, 1, 2);

  m.execute(i);

  CHECK(12 == m.get_register_value(0).to_signed32());
  CHECK(0x00 == m.get_current_program_status_register());
}

TEST_CASE("Machine, execute ADD and update zero condition flag") {
  Machine m(1024);
  m.set_register_value(1, Machine_byte(10));
  Instruction i(opcodes::ADD, condition_codes::NONE, true, update_modes::NONE,
                shift_ops::NONE, 0, 1, -10);

  m.execute(i);

  CHECK(0 == m.get_register_value(0).to_signed32());
  CHECK((BITMASK_CPSR_Z | BITMASK_CPSR_C) ==
        m.get_current_program_status_register());
}

TEST_CASE("Machine, execute ADD and update negative condition flag") {
  Machine m(1024);
  m.set_register_value(1, Machine_byte(10));
  Instruction i(opcodes::ADD, condition_codes::NONE, true, update_modes::NONE,
                shift_ops::NONE, 0, 1, -20);

  m.execute(i);

  CHECK(static_cast<uint32_t>(-10) == m.get_register_value(0).to_signed32());
  CHECK(BITMASK_CPSR_N == m.get_current_program_status_register());
}

TEST_CASE("Machine, execute ADD and update carry condition flag") {
  Machine m(1024);
  m.set_register_value(1, Machine_byte(10));
  Instruction i(opcodes::ADD, condition_codes::NONE, true, update_modes::NONE,
                shift_ops::NONE, 0, 1, std::pow(2, 32) - 1);

  m.execute(i);

  CHECK(static_cast<uint32_t>(std::pow(2, 32) + 9) ==
        m.get_register_value(0)
            .to_unsigned32()); // note it has overflowed the 32-bit register
  CHECK(BITMASK_CPSR_C == m.get_current_program_status_register());
}

TEST_CASE("Machine, execute ADD and update overflow condition flag") {
  Machine m(1024);
  m.set_register_value(1, Machine_byte(10));
  Instruction i(opcodes::ADD, condition_codes::NONE, true, update_modes::NONE,
                shift_ops::NONE, 0, 1, std::pow(2, 31) - 1);

  m.execute(i);

  CHECK(std::pow(2, 31) + 9 ==
        static_cast<uint32_t>(m.get_register_value(0).to_signed32()));
  CHECK(BITMASK_CPSR_V == m.get_current_program_status_register());
}

TEST_CASE("Machine, executing ADD prevented by condition code") {
  Machine m(1024);
  m.set_register_value(1, Machine_byte(10));
  Instruction i(opcodes::ADD, condition_codes::NE, true, update_modes::NONE,
                shift_ops::NONE, 0, 1, 2);

  m.set_current_program_status_register(BITMASK_CPSR_Z);
  m.execute(i);

  CHECK(0 == m.get_register_value(0).to_signed32());
  CHECK(BITMASK_CPSR_Z == m.get_current_program_status_register());
}

TEST_CASE("Machine, execute SUB") {
  Machine m(1024);
  m.set_register_value(1, Machine_byte(10));
  Instruction i(opcodes::SUB, condition_codes::NONE, false, update_modes::NONE,
                shift_ops::NONE, 0, 1, 5);

  m.execute(i);

  CHECK(5 == m.get_register_value(0).to_signed32());
  CHECK(0x00 == m.get_current_program_status_register());
}

TEST_CASE("Machine, execute SUB and update zero condition flag") {
  Machine m(1024);
  m.set_register_value(1, Machine_byte(10));
  Instruction i(opcodes::SUB, condition_codes::NONE, true, update_modes::NONE,
                shift_ops::NONE, 0, 1, 10);

  m.execute(i);

  CHECK(0 == m.get_register_value(0).to_signed32());
  CHECK((BITMASK_CPSR_Z | BITMASK_CPSR_C) ==
        m.get_current_program_status_register());
}

TEST_CASE("Machine, execute SUB and update negative condition flag") {
  Machine m(1024);
  m.set_register_value(1, Machine_byte(10));
  Instruction i(opcodes::SUB, condition_codes::NONE, true, update_modes::NONE,
                shift_ops::NONE, 0, 1, 15);

  m.execute(i);

  CHECK(-5 == m.get_register_value(0).to_signed32());
  CHECK(BITMASK_CPSR_N == m.get_current_program_status_register());
}

TEST_CASE("Machine, execute SUB and update carry condition flag") {
  Machine m(1024);
  m.set_register_value(1, Machine_byte(8));
  Instruction i(opcodes::SUB, condition_codes::NONE, true, update_modes::NONE,
                shift_ops::NONE, 0, 1, 1);

  m.execute(i);

  CHECK(7 == m.get_register_value(0).to_signed32());
  CHECK(BITMASK_CPSR_C == m.get_current_program_status_register());
}

TEST_CASE("Machine, execute SUB and update overflow condition flag") {
  Machine m(1024);
  m.set_register_value(1,
                       Machine_byte(std::numeric_limits<int32_t>::min() + 1));
  Instruction i(opcodes::SUB, condition_codes::NONE, true, update_modes::NONE,
                shift_ops::NONE, 0, 1, std::numeric_limits<int32_t>::max() - 1);

  m.execute(i);

  CHECK((std::numeric_limits<int32_t>::min() + 1 -
         std::numeric_limits<int32_t>::max() + 1) ==
        m.get_register_value(0).to_signed32());
  CHECK((BITMASK_CPSR_V | BITMASK_CPSR_N | BITMASK_CPSR_C) ==
        m.get_current_program_status_register());
}

TEST_CASE("Machine, prevent execution of SUB by condition flag") {
  Machine m(1024);
  m.set_register_value(1, Machine_byte(10));
  Instruction i(opcodes::SUB, condition_codes::NE, true, update_modes::NONE,
                shift_ops::NONE, 0, 1, 5);

  m.set_current_program_status_register(BITMASK_CPSR_Z);
  m.execute(i);

  CHECK(0 == m.get_register_value(0).to_signed32());
  CHECK(BITMASK_CPSR_Z == m.get_current_program_status_register());
}

TEST_CASE("Machine, execute AND") {
  Machine m(1024);
  m.set_register_value(1, Machine_byte(std::bitset<32>(std::string("010101"))));
  Instruction i(opcodes::AND, condition_codes::NONE, true, update_modes::NONE,
                shift_ops::NONE, 0, 1, 0b101110);

  m.set_current_program_status_register(0x00000000);
  m.execute(i);

  CHECK(0b000100 == m.get_register_value(0).to_unsigned32());
  CHECK(0x00000000 == m.get_current_program_status_register());
}

TEST_CASE("Machine, execute ORR") {
  Machine m(1024);
  m.set_register_value(1, Machine_byte(std::bitset<32>(std::string("000101"))));
  Instruction i(opcodes::ORR, condition_codes::NONE, true, update_modes::NONE,
                shift_ops::NONE, 0, 1, 0b101110);

  m.set_current_program_status_register(0x00000000);
  m.execute(i);

  CHECK(0b101111 == m.get_register_value(0).to_unsigned32());
  CHECK(0x00000000 == m.get_current_program_status_register());
}

TEST_CASE("Machine, execute exclusive or (EOR)") {
  Machine m(1024);
  m.set_register_value(1, Machine_byte(std::bitset<32>(std::string("010101"))));
  Instruction i(opcodes::EOR, condition_codes::NONE, true, update_modes::NONE,
                shift_ops::NONE, 0, 1, 0b101110);

  m.set_current_program_status_register(0x00000000);
  m.execute(i);

  CHECK(0b111011 == m.get_register_value(0).to_unsigned32());
  CHECK(0x00000000 == m.get_current_program_status_register());
}
