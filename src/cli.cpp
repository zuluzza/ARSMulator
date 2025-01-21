#include "cli.h"
#include "simulator.h"
#include <cassert>
#include <cstring>
#include <iostream>

std::string help_text(
    "Available commands:\nh: display this help\nr: run program until it's "
    "stopped\ns: run one instruction\nx{X}: run X instructions and stop\np: "
    "print register values\nm{X}: print memory at address X\nq: quit");

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
      m = Machine(std::stoi(argv[i]));
    } else if (strcmp(argv[i], "-c") == 0) {
      i++;
      assert(i < argc);
      std::string command_list = argv[i];
      std::string::size_type pos, prev_pos = 0;
      while ((pos = command_list.find(",", prev_pos)) != std::string::npos) {
        command_queue.push_back(command_list.substr(prev_pos, pos - prev_pos));
        prev_pos = pos + 1;
      }
      if (command_list.size() > prev_pos) {
        command_queue.push_back(
            command_list.substr(prev_pos, command_list.size() - prev_pos));
      }
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

std::string cli_app::get_next_command_from_queue() {
  if (command_queue.empty()) {
    return "";
  }
  const std::string next_command = command_queue.front();
  command_queue.pop_front();
  return next_command;
}

int main(int argc, char *argv[]) {
  cli_app app;
  app.parse_cli_args(argc, argv);
  std::cout << "Welcome to ARSMulator! Please give a command to proceed (\"h\" "
               "for help)"
            << std::endl;
  bool cont = true;
  while (cont) {
    std::string command = app.get_next_command_from_queue();
    if (command.empty()) {
      std::cin >> command;
    }

    if (!app.parse_command(command)) {
      cont = false;
    }
  }
  return 0;
}