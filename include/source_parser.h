#ifndef SOURCE_PARSER_H
#define SOURCE_PARSER_H

#include "instruction.h"

#include <map>
#include <string>
#include <vector>

// forward declaration
class SourceParserTestFixture;

class SourceCodeParser {
public:
  std::vector<Instruction> parse(std::string file_name);
  friend class SourceParserTestFixture;

private:
  opcodes parse_code(std::string &line);
  bool parse_line(std::string &line, Instruction &result,
                  std::pair<std::string, unsigned int> &unsolved_label_info);
  std::vector<uint8_t> parse_registers(std::string &line, Instruction &result,
                                       bool &unsolved_label);

  std::map<std::string, unsigned int> symbol_address_table;
  unsigned int line_number;
  std::vector<std::pair<std::string, unsigned int>> unsolved_labels;
};

#endif // SOURCE_PARSER_H