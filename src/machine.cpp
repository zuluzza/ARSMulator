#include "machine.h"
#include "instruction.h"

#include <cassert>
#include <iostream>
#include <limits>

#define REGISTER_COUNT 16

Machine::Machine(int mem_size)
{
	memory = static_cast<char*>(malloc(mem_size * sizeof(char)));
	// Fail if it was not able to reserve memory
	assert(memory);

	registers = std::vector<int32_t>(REGISTER_COUNT, 0);
	current_program_status_register = 0;
}

Machine::~Machine()
{
	if (memory)
	{
		free(memory);
	}
}

void Machine::execute(Instruction i)
{
	switch(i.get_opcode())
	{
		case opcodes::ADD:
			execute_add(i);
			break;
		case opcodes::NONE:
			// TODO add a log?
		default:
			assert(false);
	}
}

void Machine::execute_add(Instruction i)
{
	assert(i.get_opcode() == opcodes::ADD);

	const uint32_t register_to_write = i.get_register_1();
	assert(register_to_write < REGISTER_COUNT);

	const int64_t result = static_cast<int64_t>(registers[i.get_register_2()]) + static_cast<int64_t>(i.get_second_operand());
	registers[register_to_write] = result;

	if (i.get_update_condition_flags())
	{
		current_program_status_register |= ((registers[register_to_write] < 0) << SHIFT_CPRS_N);
		current_program_status_register |= ((registers[register_to_write] == 0) << SHIFT_CPRS_Z);
		
		// TODO these two are probably wrong??
		current_program_status_register |= (((result > std::numeric_limits<int32_t>::max()) || (result < std::numeric_limits<int32_t>::min())) << SHIFT_CPRS_C);
		current_program_status_register |= (((result > std::numeric_limits<int32_t>::max()) || (result < std::numeric_limits<int32_t>::min())) << SHIFT_CPRS_V);
	}
}

void Machine::set_register_value(uint8_t reg_number, int32_t value)
{
	assert(reg_number < REGISTER_COUNT);
	registers[reg_number] = value;
}

int32_t Machine::get_register_value(uint8_t reg_number)
{
	assert(reg_number < REGISTER_COUNT);
	return registers[reg_number];
}

void Machine::print_registers()
{
	for (uint8_t i = 0; i < REGISTER_COUNT; ++i)
	{
		std::cout << "Register " << i << ": " << registers[i] << std::endl;
	}
}

uint32_t Machine::get_current_program_status_register()
{
	return current_program_status_register;
}
