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
  bool parse_line(std::string &line, Instruction &result);

  std::map<std::string, unsigned int> symbol_address_table;
  unsigned int line_number;
};

#endif // SOURCE_PARSER_H