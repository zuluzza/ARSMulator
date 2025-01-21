#include "source_parser.h"

#include <cassert>
#include <cctype>
#include <fstream>
#include <iostream>
#include <map>

std::map<std::string, opcodes> opcode_table{
    {"ADC", opcodes::ADC}, {"ADD", opcodes::ADD}, {"AND", opcodes::AND},
    {"B", opcodes::B},     {"BIC", opcodes::BIC}, {"BL", opcodes::BL},
    {"CMN", opcodes::CMN}, {"CMP", opcodes::CMP}, {"EOR", opcodes::EOR},
    {"LDM", opcodes::LDM}, {"LDR", opcodes::LDR}, {"MOV", opcodes::MOV},
    {"MVN", opcodes::MVN}, {"ORR", opcodes::ORR}, {"RSB", opcodes::RSB},
    {"RSC", opcodes::RSC}, {"SBC", opcodes::SBC}, {"STM", opcodes::STM},
    {"STR", opcodes::STR}, {"SUB", opcodes::SUB}, {"SWI", opcodes::SWI},
    {"TEQ", opcodes::TEQ}, {"TST", opcodes::TST}};

std::map<std::string, condition_codes> condition_code_table{
    {"AL", condition_codes::AL}, {"EQ", condition_codes::EQ},
    {"NE", condition_codes::NE}, {"CS", condition_codes::CS},
    {"CC", condition_codes::CC}, {"MI", condition_codes::MI},
    {"PL", condition_codes::PL}, {"VS", condition_codes::VS},
    {"VC", condition_codes::VC}, {"HI", condition_codes::HI},
    {"LS", condition_codes::LS}, {"GE", condition_codes::GE},
    {"LT", condition_codes::LT}, {"GT", condition_codes::GT},
    {"LE", condition_codes::LE}};

std::map<std::string, update_modes> update_mode_table{{"IA", update_modes::IA},
                                                      {"IB", update_modes::IB},
                                                      {"DA", update_modes::DA},
                                                      {"DB", update_modes::DB}};

std::map<std::string, suffixes> suffix_table{
    {"S", suffixes::S}, {"B", suffixes::B},   {"SH", suffixes::SH},
    {"H", suffixes::H}, {"SB", suffixes::SB}, {"D", suffixes::D}};

std::vector<Instruction> SourceCodeParser::parse(std::string file_name) {
  std::ifstream asm_file_in;
  std::string instruction_line;

  asm_file_in.open(file_name);
  std::vector<Instruction> parsed_program;
  while (asm_file_in) {
    getline(asm_file_in, instruction_line);
    Instruction read_instruction;
    std::pair<std::string, unsigned int> unsolved_label_info;
    const bool is_new_instruction =
        parse_line(instruction_line, read_instruction, unsolved_label_info);
    if (is_new_instruction) {
      parsed_program.push_back(read_instruction);
    }
    if (!unsolved_label_info.first.empty()) {
      unsolved_labels.push_back(unsolved_label_info);
    }
  }
  asm_file_in.close();

  for (auto label_info : unsolved_labels) {
    auto it = symbol_address_table.find(label_info.first);
    assert(it != symbol_address_table.end());

    parsed_program[label_info.second].append_to_registers(it->second);
  }

  return parsed_program;
}

static std::string remove_leading_spaces(std::string const& line) {
  const auto first_non_whitespace = line.find_first_not_of(' ');
  return line.substr(first_non_whitespace, line.size() - first_non_whitespace);
}

std::vector<uint8_t> SourceCodeParser::parse_registers(std::string &line,
                                                       Instruction &result,
                                                       bool &unsolved_label) {
  unsolved_label = false;
  std::vector<uint8_t> register_list;
  while (line.size() > 0) {
    line = remove_leading_spaces(line);
    switch (line[0]) {
    case '{': // register list begins
      line = line.substr(1, line.size() - 1);
    // intenional fall-through
    case 'r': // register
      line = line.substr(1, line.size() - 1);
      register_list.push_back(std::stoi(line));
      break;
    case '-': // register range
    {
      line = line.substr(2, line.size() - 2);
      uint8_t end_value = std::stoi(line);
      for (uint8_t i = register_list.back() + 1; i <= end_value; ++i) {
        register_list.push_back(i);
      }
    } break;
    case '#': // immediate
      line = line.substr(1, line.size() - 1);
      if (line.size() > 1 && line[1] == 'x') {
        // hexadecimal value
        result.set_second_operand(
            std::stoi(line.substr(2, line.size() - 2), nullptr, 16));
      } else {
        result.set_second_operand(std::stoi(line));
      }
      break;
    default:
      break;
    }

    auto next_pos = line.find_first_of("r#-{;");
    if (next_pos == std::string::npos || line[next_pos] == ';') {
      break;
    }
    line = line.substr(next_pos, line.size() - next_pos);
  }
  if (!line.empty() && line[0] != ';' && !isdigit(line[0])) {
    // it's a label
    auto next_pos = line.find_first_of(" ;,");
    if (next_pos != std::string::npos) {
      line = line.substr(0, next_pos);
    }
    auto item = symbol_address_table.find(line);
    if (item != symbol_address_table.end()) {
      register_list.push_back(symbol_address_table[line]);
    } else {
      unsolved_label = true;
    }
  }
  return register_list;
}

bool SourceCodeParser::parse_line(
    std::string &line, Instruction &result,
    std::pair<std::string, unsigned int> &unsolved_label_info) {
  if (line[0] == '@' || line[0] == ';') {
    // it's comment line
    return false;
  }

  if (line[0] != ' ') {
    // it's a label
    symbol_address_table[line] = line_number;
    line_number++;
    return false;
  }
  line = remove_leading_spaces(line);

  // parse opcode
  int n = 3;
  auto found_opcode = opcode_table.end();
  do {
    found_opcode = opcode_table.find(line.substr(0, n));
    n--;
  } while (found_opcode == opcode_table.end() && n > 0);

  if (found_opcode == opcode_table.end()) {
    std::cout << "Unknown opcode in: " << line << std::endl;
    return false;
  }

  result.set_opcode(found_opcode->second);
  line = line.substr(n + 1, line.size() - n);

  // parse condition code if it exists
  auto found_condition_code = condition_code_table.find(line.substr(0, 2));
  if (found_condition_code != condition_code_table.end()) {
    line = line.substr(2, line.size() - 2);
    result.set_condition_code(found_condition_code->second);
  }

  // parse update mode if it exists
  auto found_update_mode = update_mode_table.find(line.substr(0, 2));
  if (found_update_mode != update_mode_table.end()) {
    line = line.substr(2, line.size() - 2);
    result.set_update_mode(found_update_mode->second);
  }

  // parse suffix if it exists
  n = 2;
  auto found_suffix = suffix_table.end();
  do {
    found_suffix = suffix_table.find(line.substr(0, n));
    n--;
  } while (found_suffix == suffix_table.end() && n > 0);
  if (found_suffix != suffix_table.end()) {
    line = line.substr(n + 1, line.size() - n);
    result.set_suffix(found_suffix->second);
  }

  // parse registers
  bool unsolved_label;
  std::vector<uint8_t> register_list =
      parse_registers(line, result, unsolved_label);
  if (unsolved_label) {
    unsolved_label_info.first = line;
    unsolved_label_info.second = line_number;
  }
  result.set_registers(register_list);

  line_number++;
  return true;
}
