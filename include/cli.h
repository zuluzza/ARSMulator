#include "instruction.h"
#include "machine.h"
#include "source_parser.h"

#include <list>
#include <vector>

class cli_app {
public:
  cli_app() : m(256), program({}), file_name(""), source_parser(){};
  void parse_cli_args(int argc, char *argv[]);
  bool parse_command(std::string &command);
  void run(int count = 0);
  std::string get_next_command_from_queue();

private:
  Machine m;
  std::vector<Instruction> program;
  std::string file_name;
  SourceCodeParser source_parser;
  std::list<std::string> command_queue;
};
