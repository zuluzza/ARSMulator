#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>

#include "instruction.h"
#include "source_parser.h"
#include <fstream>
#include <string>

class SourceParserTestFixture {
public:
  SourceParserTestFixture() : parser() {}

  void verify_symbol(std::string symbol, unsigned int expected) {
    CHECK(parser.symbol_address_table[symbol] == expected);
  }

  void parse_line_and_check_return_value(std::string &test_line, Instruction &i,
                                         bool expected) {
    CHECK(expected == parser.parse_line(test_line, i));
  }

  std::vector<Instruction> parse_file(std::string file_name) {
    return parser.parse(file_name);
  }

protected:
  SourceCodeParser parser;
};

TEST_CASE_METHOD(SourceParserTestFixture, "parse symbol") {
  std::string test_line("symbol");
  Instruction i;
  parse_line_and_check_return_value(test_line, i, false);
  verify_symbol("symbol", 0);
}

TEST_CASE_METHOD(SourceParserTestFixture, "remove leading whitespace") {
  std::string test_line("    ADD r1, r2, #30");
  Instruction i;
  parse_line_and_check_return_value(test_line, i, false);
  CHECK(0 == test_line.compare("ADD r1, r2, #30"));
}

TEST_CASE_METHOD(SourceParserTestFixture, "ADD with condition code") {
  std::string test_line("    ADDEQ r1, r2, #30");
  Instruction i;
  parse_line_and_check_return_value(test_line, i, true);
  CHECK(i.get_opcode() == opcodes::ADD);
  CHECK(i.get_condition_code() == condition_codes::EQ);
  CHECK(i.is_2nd_operand_register() == false);
  CHECK(i.get_second_operand() == 30);
  REQUIRE(i.get_register_count() == 2);
  CHECK(i.get_register(0) == 1);
  CHECK(i.get_register(1) == 2);
}

TEST_CASE_METHOD(SourceParserTestFixture, "ADD with suffix") {
  std::string test_line("    ADDS r1, r2, #30");
  Instruction i;
  parse_line_and_check_return_value(test_line, i, true);
  CHECK(i.get_opcode() == opcodes::ADD);
  CHECK(i.get_suffix() == suffixes::S);
  CHECK(i.is_2nd_operand_register() == false);
  CHECK(i.get_second_operand() == 30);
  REQUIRE(i.get_register_count() == 2);
  CHECK(i.get_register(0) == 1);
  CHECK(i.get_register(1) == 2);
}

TEST_CASE_METHOD(SourceParserTestFixture,
                 "ADD with condition code and suffix") {
  std::string test_line("    ADDEQS r1, r2, #30");
  Instruction i;
  parse_line_and_check_return_value(test_line, i, true);
  CHECK(i.get_opcode() == opcodes::ADD);
  CHECK(i.get_condition_code() == condition_codes::EQ);
  CHECK(i.get_suffix() == suffixes::S);
  CHECK(i.is_2nd_operand_register() == false);
  CHECK(i.get_second_operand() == 30);
  REQUIRE(i.get_register_count() == 2);
  CHECK(i.get_register(0) == 1);
  CHECK(i.get_register(1) == 2);
}

TEST_CASE_METHOD(SourceParserTestFixture,
                 "LDM with update mode and condition code") {
  std::string test_line("    LDMEQIA r1, r2, #30");
  Instruction i;
  parse_line_and_check_return_value(test_line, i, true);
  CHECK(i.get_opcode() == opcodes::LDM);
  CHECK(i.get_condition_code() == condition_codes::EQ);
  CHECK(i.get_update_mode() == update_modes::IA);
  CHECK(i.is_2nd_operand_register() == false);
  CHECK(i.get_second_operand() == 30);
  REQUIRE(i.get_register_count() == 2);
  CHECK(i.get_register(0) == 1);
  CHECK(i.get_register(1) == 2);
}

TEST_CASE_METHOD(SourceParserTestFixture, "LDR with byte") {
  std::string test_line("    LDRB r1, r2, #30");
  Instruction i;
  parse_line_and_check_return_value(test_line, i, true);
  CHECK(i.get_opcode() == opcodes::LDR);
  CHECK(i.get_suffix() == suffixes::B);
  CHECK(i.is_2nd_operand_register() == false);
  CHECK(i.get_second_operand() == 30);
  REQUIRE(i.get_register_count() == 2);
  CHECK(i.get_register(0) == 1);
  CHECK(i.get_register(1) == 2);
}

TEST_CASE_METHOD(SourceParserTestFixture, "SUB with immediate") {
  std::string test_line("    SUB, r3, r5, #200");
  Instruction i;
  parse_line_and_check_return_value(test_line, i, true);
  CHECK(i.get_opcode() == opcodes::SUB);
  CHECK(i.is_2nd_operand_register() == false);
  CHECK(i.get_second_operand() == 200);
  REQUIRE(i.get_register_count() == 2);
  CHECK(i.get_register(0) == 3);
  CHECK(i.get_register(1) == 5);
}

TEST_CASE_METHOD(SourceParserTestFixture, "Comment line is ignored") {
  std::string test_line(";this is a comment");
  Instruction i;
  parse_line_and_check_return_value(test_line, i, false);
}

TEST_CASE_METHOD(SourceParserTestFixture,
                 "Comment at the end of line is ignored") {
  std::string test_line("    SUB, r3, r5, #200 ;this is comment");
  Instruction i;
  parse_line_and_check_return_value(test_line, i, true);
  CHECK(i.get_opcode() == opcodes::SUB);
  CHECK(i.is_2nd_operand_register() == false);
  CHECK(i.get_second_operand() == 200);
  REQUIRE(i.get_register_count() == 2);
  CHECK(i.get_register(0) == 3);
  CHECK(i.get_register(1) == 5);
}

TEST_CASE_METHOD(SourceParserTestFixture, "Parse a file") {
  std::ofstream asm_file;
  std::string file_name = "test_file1234.s";
  asm_file.open(file_name);
  asm_file << ";This is a test program" << std::endl;
  asm_file << "    MOV r1, #123" << std::endl;
  asm_file << "    MOV r2, #35" << std::endl;
  asm_file << "    ADD r0, r1, r2 ; 123+35" << std::endl;
  asm_file << "    SUB r5, r0, r2 ; 123+35-35" << std::endl;
  asm_file << "    STR r2, r5 ;Stores the value of r2 to the address in r5"
           << std::endl;
  asm_file.close();

  auto parsed_program = parse_file(file_name);
  CHECK(parsed_program[0].get_opcode() == opcodes::MOV);
  CHECK(parsed_program[1].get_opcode() == opcodes::MOV);
  CHECK(parsed_program[2].get_opcode() == opcodes::ADD);
  CHECK(parsed_program[3].get_opcode() == opcodes::SUB);
  CHECK(parsed_program[4].get_opcode() == opcodes::STR);
}
