#include "instruction.h"
	
Instruction::Instruction(opcodes operation, condition_codes condition, bool update_condition_code_flags, update_modes update, shift_ops shift, uint32_t reg1, uint32_t reg2, int32_t second_operand)
{
	opcode = operation;
	condition_code = condition;
	update_flags = update_condition_code_flags;
	update_mode = update;
	shift_op = shift;
	register_1 = reg1;
	register_2 = reg2;
	flex_2nd_operand = second_operand;
}

opcodes Instruction::get_opcode()
{
	return opcode;
}

bool Instruction::has_condition_code()
{
	return condition_code == condition_codes::NONE;
}

bool Instruction::has_updade_mode()
{
	return update_mode == update_modes::NONE;
}

bool Instruction::has_shift_op()
{
	return shift_op == shift_ops::NONE;
}

uint32_t Instruction::get_register_1()
{
	return register_1;
}
uint32_t Instruction::get_register_2()
{
	return register_2;
}

int32_t Instruction::get_second_operand()
{
	return flex_2nd_operand;
}

bool Instruction::get_update_condition_flags()
{
	return update_flags;
}

condition_codes Instruction::get_condition_code()
{
	return condition_code;
}