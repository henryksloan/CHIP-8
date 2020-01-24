#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

class Disassembler {
 public:
    void disassemble(std::ifstream &file);
    std::vector<std::string> get_instructions() { return instructions; }

 private:
    std::vector<std::string> instructions;
};

#endif