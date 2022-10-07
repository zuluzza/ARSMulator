#include "cli.h"
#include "simulator.h"
#include <cassert>
#include <cstring>
#include <iostream>

std::string help_text(
    "Available commands:\nh: display this help\nr: run program until it's "
    "stopped\ns: run one instruction\nx{X}: run X instructions and stop\np: "
    "print register values\nm{X}: print memory at address X");

void cli_app::parse_cli_args(int argc, char *argv[]) {
  int i = 0;
  while (i < argc) {
    if (strcmp(argv[i], "-f") == 0) {
      i++;
      assert(i < argc);
      program = source_parser.parse(argv[i]);
    } else if (strcmp(argv[i], "-m") == 0) {
      i++;
      assert(i < argc);
      m.~Machine();
      m = Machine(std::stoi(argv[i]));
    }
    i++;
  }
}

bool cli_app::parse_command(std::string &command) {
  if (command.c_str()[0] == 'h') {
    std::cout << help_text << std::endl;
  } else if (command.c_str()[0] == 'r') {
    run();
  } else if (command.c_str()[0] == 's') {
    run(1);
  } else if (command.c_str()[0] == 'x') {
    run(std::stoi(&command[1]));
  } else if (command.c_str()[0] == 'p') {
    m.print_registers();
  } else if (command.c_str()[0] == 'm') {
    std::cout << m.get_memory(std::stoi(&command[1])).to_unsigned32()
              << std::endl;
  } else if (command.c_str()[0] == 'q') {
    std::cout << "Thanks for ARSMulating! Have a nice day!" << std::endl;
    return false;
  }
  return true;
}

void cli_app::run(int count) {
  if (program.empty()) {
    std::cout << "Please insert a program before running!" << std::endl;
    return;
  }
  Simulator::run_program(program, m, count);
}

int main(int argc, char *argv[]) {
  cli_app app;
  app.parse_cli_args(argc, argv);
  std::cout << "Welcome to ARSMulator! Please give a command to proceed (\"h\" "
               "for help)"
            << std::endl;
  bool cont = true;
  while (cont) {
    std::string command;
    std::cin >> command;
    if (!app.parse_command(command)) {
      break;
    }
  }
  return 0;
}