#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>

#include "instruction.h"
#include "machine.h"

#include <cmath>

class MachineTestFixture {
public:
  MachineTestFixture() : m(1024) {}

protected:
  Machine m;
};

TEST_CASE_METHOD(MachineTestFixture, "meets_condition_code->true") {
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

TEST_CASE_METHOD(MachineTestFixture, "meets_condition_code->false") {
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

TEST_CASE_METHOD(MachineTestFixture, "execute ADD") {
  m.set_register_value(1, Machine_byte(10));
  Instruction i(opcodes::ADD, condition_codes::NONE, suffixes::NONE,
                update_modes::NONE, {0, 1}, 2);

  m.execute(i);

  CHECK(12 == m.get_register_value(0).to_signed32());
  CHECK(0x00 == m.get_current_program_status_register());
}

TEST_CASE_METHOD(MachineTestFixture,
                 "execute ADD and update zero condition flag") {
  m.set_register_value(1, Machine_byte(10));
  Instruction i(opcodes::ADD, condition_codes::NONE, suffixes::S,
                update_modes::NONE, {0, 1}, -10);

  m.execute(i);

  CHECK(0 == m.get_register_value(0).to_signed32());
  CHECK((BITMASK_CPSR_Z | BITMASK_CPSR_C) ==
        m.get_current_program_status_register());
}

TEST_CASE_METHOD(MachineTestFixture,
                 "execute ADD and update negative condition flag") {
  m.set_register_value(1, Machine_byte(10));
  Instruction i(opcodes::ADD, condition_codes::NONE, suffixes::S,
                update_modes::NONE, {0, 1}, -20);

  m.execute(i);

  CHECK(static_cast<uint32_t>(-10) == m.get_register_value(0).to_signed32());
  CHECK(BITMASK_CPSR_N == m.get_current_program_status_register());
}

TEST_CASE_METHOD(MachineTestFixture,
                 "execute ADD and update carry condition flag") {
  m.set_register_value(1, Machine_byte(10));
  Instruction i(opcodes::ADD, condition_codes::NONE, suffixes::S,
                update_modes::NONE, {0, 1}, std::pow(2, 32) - 1);

  m.execute(i);

  CHECK(9 ==
        m.get_register_value(0)
            .to_unsigned32()); // note it has overflowed the 32-bit register
  CHECK(BITMASK_CPSR_C == m.get_current_program_status_register());
}

TEST_CASE_METHOD(MachineTestFixture,
                 "execute ADD and update overflow condition flag") {
  m.set_register_value(1, Machine_byte(10));
  Instruction i(opcodes::ADD, condition_codes::NONE, suffixes::S,
                update_modes::NONE, {0, 1}, std::pow(2, 31) - 1);

  m.execute(i);

  CHECK(std::pow(2, 31) + 9 ==
        static_cast<uint32_t>(m.get_register_value(0).to_signed32()));
  CHECK(BITMASK_CPSR_V == m.get_current_program_status_register());
}

TEST_CASE_METHOD(MachineTestFixture,
                 "executing ADD prevented by condition code") {
  m.set_register_value(1, Machine_byte(10));
  Instruction i(opcodes::ADD, condition_codes::NE, suffixes::S,
                update_modes::NONE, {0, 1}, 2);

  m.set_current_program_status_register(BITMASK_CPSR_Z);
  m.execute(i);

  CHECK(0 == m.get_register_value(0).to_signed32());
  CHECK(BITMASK_CPSR_Z == m.get_current_program_status_register());
}

TEST_CASE_METHOD(MachineTestFixture, "execute SUB") {
  m.set_register_value(1, Machine_byte(10));
  Instruction i(opcodes::SUB, condition_codes::NONE, suffixes::NONE,
                update_modes::NONE, {0, 1}, 5);

  m.execute(i);

  CHECK(5 == m.get_register_value(0).to_signed32());
  CHECK(0x00 == m.get_current_program_status_register());
}

TEST_CASE_METHOD(MachineTestFixture,
                 "execute SUB and update zero condition flag") {
  m.set_register_value(1, Machine_byte(10));
  Instruction i(opcodes::SUB, condition_codes::NONE, suffixes::S,
                update_modes::NONE, {0, 1}, 10);

  m.execute(i);

  CHECK(0 == m.get_register_value(0).to_signed32());
  CHECK((BITMASK_CPSR_Z | BITMASK_CPSR_C) ==
        m.get_current_program_status_register());
}

TEST_CASE_METHOD(MachineTestFixture,
                 "execute SUB and update negative condition flag") {
  m.set_register_value(1, Machine_byte(10));
  Instruction i(opcodes::SUB, condition_codes::NONE, suffixes::S,
                update_modes::NONE, {0, 1}, 15);

  m.execute(i);

  CHECK(-5 == m.get_register_value(0).to_signed32());
  CHECK(BITMASK_CPSR_N == m.get_current_program_status_register());
}

TEST_CASE_METHOD(MachineTestFixture,
                 "execute SUB and update carry condition flag") {
  m.set_register_value(1, Machine_byte(8));
  Instruction i(opcodes::SUB, condition_codes::NONE, suffixes::S,
                update_modes::NONE, {0, 1}, 1);

  m.execute(i);

  CHECK(7 == m.get_register_value(0).to_signed32());
  CHECK(BITMASK_CPSR_C == m.get_current_program_status_register());
}

TEST_CASE_METHOD(MachineTestFixture,
                 "execute SUB and update overflow condition flag") {
  m.set_register_value(1,
                       Machine_byte(std::numeric_limits<int32_t>::min() + 1));
  Instruction i(opcodes::SUB, condition_codes::NONE, suffixes::S,
                update_modes::NONE, {0, 1},
                std::numeric_limits<int32_t>::max() - 1);

  m.execute(i);

  CHECK((std::numeric_limits<int32_t>::min() + 1 -
         std::numeric_limits<int32_t>::max() + 1) ==
        m.get_register_value(0).to_signed32());
  CHECK((BITMASK_CPSR_V | BITMASK_CPSR_N | BITMASK_CPSR_C) ==
        m.get_current_program_status_register());
}

TEST_CASE_METHOD(MachineTestFixture,
                 "prevent execution of SUB by condition flag") {
  m.set_register_value(1, Machine_byte(10));
  Instruction i(opcodes::SUB, condition_codes::NE, suffixes::S,
                update_modes::NONE, {0, 1}, 5);

  m.set_current_program_status_register(BITMASK_CPSR_Z);
  m.execute(i);

  CHECK(0 == m.get_register_value(0).to_signed32());
  CHECK(BITMASK_CPSR_Z == m.get_current_program_status_register());
}

TEST_CASE_METHOD(MachineTestFixture, "execute AND") {
  m.set_register_value(1, Machine_byte(std::bitset<32>(std::string("010101"))));
  Instruction i(opcodes::AND, condition_codes::NONE, suffixes::S,
                update_modes::NONE, {0, 1}, 0b101110);

  m.set_current_program_status_register(0x00000000);
  m.execute(i);

  CHECK(0b000100 == m.get_register_value(0).to_unsigned32());
  CHECK(0x00000000 == m.get_current_program_status_register());
}

TEST_CASE_METHOD(MachineTestFixture, "execute ORR") {
  m.set_register_value(1, Machine_byte(std::bitset<32>(std::string("000101"))));
  Instruction i(opcodes::ORR, condition_codes::NONE, suffixes::S,
                update_modes::NONE, {0, 1}, 0b101110);

  m.set_current_program_status_register(0x00000000);
  m.execute(i);

  CHECK(0b101111 == m.get_register_value(0).to_unsigned32());
  CHECK(0x00000000 == m.get_current_program_status_register());
}

TEST_CASE_METHOD(MachineTestFixture, "execute exclusive or (EOR)") {
  m.set_register_value(1, Machine_byte(std::bitset<32>(std::string("010101"))));
  Instruction i(opcodes::EOR, condition_codes::NONE, suffixes::S,
                update_modes::NONE, {0, 1}, 0b101110);

  m.set_current_program_status_register(0x00000000);
  m.execute(i);

  CHECK(0b111011 == m.get_register_value(0).to_unsigned32());
  CHECK(0x00000000 == m.get_current_program_status_register());
}

TEST_CASE_METHOD(MachineTestFixture,
                 "execute CMP and update zero condition flag") {
  m.set_register_value(0, Machine_byte(10));
  m.set_register_value(1, Machine_byte(10));
  Instruction i(opcodes::CMP, condition_codes::NONE, suffixes::NONE,
                update_modes::NONE, {0, 1}, 10);

  m.execute(i);

  CHECK(10 == m.get_register_value(0).to_signed32());
  CHECK((BITMASK_CPSR_Z | BITMASK_CPSR_C) ==
        m.get_current_program_status_register());
}

TEST_CASE_METHOD(MachineTestFixture,
                 "execute CMN and update negative condition flag") {
  m.set_register_value(0, Machine_byte(10));
  m.set_register_value(1, Machine_byte(10));
  Instruction i(opcodes::CMN, condition_codes::NONE, suffixes::NONE,
                update_modes::NONE, {0, 1}, -20);

  m.execute(i);

  CHECK(10 == m.get_register_value(0).to_signed32());
  CHECK(BITMASK_CPSR_N == m.get_current_program_status_register());
}

TEST_CASE_METHOD(MachineTestFixture, "execute TST") {
  m.set_register_value(1, Machine_byte(std::bitset<32>(std::string("010101"))));
  Instruction i(opcodes::TST, condition_codes::NONE, suffixes::S,
                update_modes::NONE, {0, 1}, 0b101010);

  m.set_current_program_status_register(0x00000000);
  m.execute(i);

  CHECK(0b000000 == m.get_register_value(0).to_unsigned32());
  CHECK(BITMASK_CPSR_Z == m.get_current_program_status_register());
}

TEST_CASE_METHOD(MachineTestFixture, "execute TEQ") {
  m.set_register_value(1, Machine_byte(std::bitset<32>(std::string("010101"))));
  Instruction i(opcodes::TEQ, condition_codes::NONE, suffixes::S,
                update_modes::NONE, {0, 1}, 0b101110);

  m.set_current_program_status_register(0x00000000);
  m.execute(i);

  CHECK(0b000000 == m.get_register_value(0).to_unsigned32());
  CHECK(0x00000000 == m.get_current_program_status_register());
}

TEST_CASE_METHOD(MachineTestFixture, "execute bit clear") {
  m.set_register_value(1, Machine_byte(std::bitset<32>(std::string("010101"))));
  Instruction i(opcodes::BIC, condition_codes::NONE, suffixes::S,
                update_modes::NONE, {0, 1}, 0b010101);

  m.set_current_program_status_register(0x00000000);
  m.execute(i);

  CHECK(0b000000 == m.get_register_value(0).to_unsigned32());
  CHECK(BITMASK_CPSR_Z == m.get_current_program_status_register());
}

TEST_CASE_METHOD(MachineTestFixture, "Load word") {
  m.set_register_value(1, Machine_byte(256));
  m.set_memory(256, Machine_byte(512));
  CHECK(512 == m.get_memory(256).to_unsigned32());
  Instruction i(opcodes::LDR, condition_codes::NONE, suffixes::NONE,
                update_modes::NONE, {0, 1}, 0);
  m.execute(i);
  CHECK(512 == m.get_register_value(0).to_unsigned32());
}

TEST_CASE_METHOD(MachineTestFixture, "Load byte") {
  m.set_register_value(1, Machine_byte(256));
  m.set_memory(256, Machine_byte(0b110100110));
  CHECK(0b110100110 == m.get_memory(256).to_unsigned32());
  Instruction i(opcodes::LDR, condition_codes::NONE, suffixes::B,
                update_modes::NONE, {0, 1}, 0);
  m.execute(i);
  CHECK(0b010100110 == m.get_register_value(0).to_unsigned32());
}

TEST_CASE_METHOD(MachineTestFixture, "Load half-word") {
  m.set_register_value(1, Machine_byte(256));
  m.set_memory(256, Machine_byte(0b11010011000100110));
  CHECK(0b11010011000100110 == m.get_memory(256).to_unsigned32());
  Instruction i(opcodes::LDR, condition_codes::NONE, suffixes::H,
                update_modes::NONE, {0, 1}, 0);
  m.execute(i);
  CHECK(0b01010011000100110 == m.get_register_value(0).to_unsigned32());
}

TEST_CASE_METHOD(MachineTestFixture, "Load double word") {
  m.set_register_value(2, Machine_byte(256));
  m.set_memory(256, Machine_byte(0x01234567));
  m.set_memory(257, Machine_byte(0x89ABCDEF));
  CHECK(0x01234567 == m.get_memory(256).to_unsigned32());
  CHECK(0x89ABCDEF == m.get_memory(257).to_unsigned32());
  Instruction i(opcodes::LDR, condition_codes::NONE, suffixes::D,
                update_modes::NONE, {0, 2}, 0);
  m.execute(i);
  CHECK(0x01234567 == m.get_register_value(0).to_unsigned32());
  CHECK(0x89ABCDEF == m.get_register_value(1).to_unsigned32());
}

TEST_CASE_METHOD(MachineTestFixture, "Store word") {
  m.set_register_value(1, Machine_byte(256));
  m.set_register_value(0, Machine_byte(0x01234567));
  Instruction i(opcodes::STR, condition_codes::NONE, suffixes::NONE,
                update_modes::NONE, {0, 1}, 0);
  m.execute(i);
  CHECK(0x01234567 == m.get_memory(256).to_unsigned32());
}

TEST_CASE_METHOD(MachineTestFixture, "Store half-word") {
  m.set_register_value(1, Machine_byte(256));
  m.set_register_value(0, Machine_byte(0x01234567));
  Instruction i(opcodes::STR, condition_codes::NONE, suffixes::H,
                update_modes::NONE, {0, 1}, 0);
  m.execute(i);
  CHECK((0x01234567 & 0xFFFF) == m.get_memory(256).to_unsigned32());
}

TEST_CASE_METHOD(MachineTestFixture, "Store byte") {
  m.set_register_value(1, Machine_byte(256));
  m.set_register_value(0, Machine_byte(0x01234567));
  Instruction i(opcodes::STR, condition_codes::NONE, suffixes::B,
                update_modes::NONE, {0, 1}, 0);
  m.execute(i);
  CHECK((0x01234567 & 0xFF) == m.get_memory(256).to_unsigned32());
}

TEST_CASE_METHOD(MachineTestFixture, "Store double word") {
  m.set_register_value(2, Machine_byte(256));
  m.set_register_value(0, Machine_byte(0x01234567));
  m.set_register_value(1, Machine_byte(0x89ABCDEF));
  Instruction i(opcodes::STR, condition_codes::NONE, suffixes::D,
                update_modes::NONE, {0, 2}, 0);
  m.execute(i);
  CHECK(0x01234567 == m.get_memory(256).to_unsigned32());
  CHECK(0x89ABCDEF == m.get_memory(257).to_unsigned32());
}

TEST_CASE_METHOD(MachineTestFixture, "move") {
  Instruction i(opcodes::MOV, condition_codes::NONE, suffixes::S,
                update_modes::NONE, {0, 0}, 123);
  m.execute(i);
  CHECK(123 == m.get_register_value(0).to_unsigned32());
}

TEST_CASE_METHOD(MachineTestFixture, "move not") {
  Instruction i(opcodes::MVN, condition_codes::NONE, suffixes::S,
                update_modes::NONE, {0, 2}, 123);
  m.execute(i);
  CHECK((~123) == m.get_register_value(0).to_unsigned32());
}

TEST_CASE_METHOD(MachineTestFixture, "branch") {
  Instruction i(opcodes::B, condition_codes::NONE, suffixes::NONE,
                update_modes::NONE, {0}, 5);
  m.execute(i);
  CHECK(5 == m.get_register_value(15).to_unsigned32());
}

TEST_CASE_METHOD(MachineTestFixture, "branch with link") {
  Instruction i(opcodes::BL, condition_codes::NONE, suffixes::NONE,
                update_modes::NONE, {0}, 5);
  m.set_register_value(15, 10);
  m.execute(i);
  CHECK(5 == m.get_register_value(15).to_unsigned32());
  CHECK(11 == m.get_register_value(14).to_unsigned32());
}

TEST_CASE_METHOD(MachineTestFixture, "Load multiple") {
  Instruction i(opcodes::LDM, condition_codes::NONE, suffixes::NONE,
                update_modes::IA, {0, 3, 5, 8}, 0);
  m.set_register_value(0, 256);
  m.set_memory(256, 1);
  m.set_memory(257, 2);
  m.set_memory(258, 3);
  m.execute(i);
  CHECK(1 == m.get_register_value(3).to_unsigned32());
  CHECK(2 == m.get_register_value(5).to_unsigned32());
  CHECK(3 == m.get_register_value(8).to_unsigned32());
}

TEST_CASE_METHOD(MachineTestFixture, "Store multiple") {
  Instruction i(opcodes::LDM, condition_codes::NONE, suffixes::NONE,
                update_modes::DA, {0, 3, 5, 8}, 0);
  m.set_register_value(0, 258);
  m.set_memory(256, 1);
  m.set_memory(257, 2);
  m.set_memory(258, 3);
  m.execute(i);
  CHECK(3 == m.get_register_value(3).to_unsigned32());
  CHECK(2 == m.get_register_value(5).to_unsigned32());
  CHECK(1 == m.get_register_value(8).to_unsigned32());
}