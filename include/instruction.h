#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <string>

enum class opcodes
{
	NONE = 0,
	ADD = 1,
	SUB = 2,
	RSB = 3,
	ADC = 4,
	SBC = 5,
	RSC = 6,
	MUL = 7,
	MLA = 8,
	UMULL = 9,
	UMLAL = 10,
	SMULL = 11,
	SMLAL = 12
};

enum class condition_codes
{
	NONE = 0,
	AL = 1,
	EQ = 2,
	NE = 3,
	CS = 4,
	CC = 5,
	MI = 6,
	PL = 7,
	VS = 8,
	VC = 9,
	HS = 10,
	LO = 11,
	HI = 12,
	LS = 13,
	GE = 14,
	LT = 15,
	GT = 16,
	LE = 17
};

enum class update_modes
{
	NONE = 0,
	FA = 1,
	EA = 2,
	FD = 3,
	ED = 4
};

enum class shift_ops
{
	NONE = 0,
	LSL = 1,
	LSR = 2,
	ASR = 3,
	ROR = 4,
	RRX = 5
};

class Instruction
{
public:
	Instruction(opcodes operation, condition_codes condition, bool update_condition_code_flags, update_modes update, shift_ops shift, uint32_t reg1, uint32_t reg2, int32_t second_operand);
	Instruction(const Instruction& i) = default;

	bool has_condition_code();
	bool has_updade_mode();
	bool has_shift_op();

	opcodes get_opcode();
	uint32_t get_register_1();
	uint32_t get_register_2();
	int32_t get_second_operand();
	bool get_update_condition_flags();

private:
	opcodes opcode;
	condition_codes condition_code;
	bool update_flags;
	update_modes update_mode;
	shift_ops shift_op;
	uint32_t register_1;
	uint32_t register_2;
	int32_t flex_2nd_operand;
};

#endif // INSTRUCTION_H