#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>

#include "instruction.h"
#include "machine.h"

TEST_CASE("Machine, execute ADD")
{
	Machine m(1024);
	m.set_register_value(1, 10);
	Instruction i(opcodes::ADD, condition_codes::NONE, false, update_modes::NONE, shift_ops::NONE, 0, 1, 2);

	m.execute(i);

	REQUIRE(12 == m.get_register_value(0));
	REQUIRE(0x00 == m.get_current_program_status_register());
}

TEST_CASE("Machine, execute ADD and update zero condition flag")
{
	Machine m(1024);
	m.set_register_value(1, 10);
	Instruction i(opcodes::ADD, condition_codes::NONE, true, update_modes::NONE, shift_ops::NONE, 0, 1, -10);

	m.execute(i);

	REQUIRE(0 == m.get_register_value(0));
	REQUIRE(BITMASK_CPSR_Z == m.get_current_program_status_register());
}

TEST_CASE("Machine, execute ADD and update negative condition flag")
{
	Machine m(1024);
	m.set_register_value(1, 10);
	Instruction i(opcodes::ADD, condition_codes::NONE, true, update_modes::NONE, shift_ops::NONE, 0, 1, -20);

	m.execute(i);

	REQUIRE(static_cast<uint32_t>(-10) == m.get_register_value(0));
	REQUIRE(BITMASK_CPSR_N == m.get_current_program_status_register());
}