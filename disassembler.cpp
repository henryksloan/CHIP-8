#include "disassembler.h"

void Disassembler::disassemble(std::ifstream &file) {
    char a, b;
    unsigned short instr;
    std::string opcode;
    std::string args;
    while (file.read(&a, 1)) {
        if (!file.read(&b, 1)) {
            // TODO
        }
        instr = ((unsigned char) a) << 8 | ((unsigned char) b);

        switch (instr & 0xF000) {
            case 0x0000:
                switch (instr & 0x00FF) {
                    case 0xE0:
                        opcode = "CLS";
                        args = "";
                        break;
                    case 0xEE:
                        opcode = "RET";
                        args = "";
                        break;
                    default:
                        opcode = "";
                        args = "";
                        break;
                }
                break;
            case 0x1000:
                opcode = "JP";
                args = "n";
                break;
            case 0x2000:
                opcode = "CALL";
                args = "n";
                break;
            case 0x3000:
                opcode = "SE";
                args = "xk";
                break;
            case 0x4000:
                opcode = "SNE";
                args = "xk";
                break;
            case 0x5000:
                opcode = "SE";
                args = "xy";
                break;
            case 0x6000:
                opcode = "LD";
                args = "xk";
                break;
            case 0x7000:
                opcode = "ADD";
                args = "xk";
                break;
            case 0x8000: {
                unsigned char suff = (instr & 0x000F);
                if ((suff > 0) && (suff <= 7)) {
                    opcode = (std::vector<std::string>){"LD", "OR", "AND", "XOR", "ADD", "SUB", "SHR", "SUBN"}[suff];
                }
                else if (suff == 0xE) {
                    opcode = "SHL";
                }
                else {
                    // TODO
                }
                args = (suff == 6 || suff == 0xE) ? "x" : "xy";
                }
                break;
            case 0x9000:
                opcode = "SNE";
                args = "xy";
                break;
            case 0xA000:
                opcode = "LD";
                args = "In";
                break;
            case 0xB000:
                opcode = "JP";
                args = "0n";
                break;
            case 0xC000:
                opcode = "RND";
                args = "xk";
                break;
            case 0xD000:
                opcode = "DRW";
                args = "xyN";
                break;
            case 0xE000: {
                short suff = instr & 0x00FF;
                if (suff == 0x9E) {
                    opcode = "SKP";
                    args = "x";
                }
                else if (suff == 0xA1) {
                    opcode = "SKNP";
                    args = "x";
                }
                else {
                    // TODO
                }
                }
                break;
            case 0xF000:
                switch (instr & 0x00FF) {
                    case 0x0007:
                        opcode = "LD";
                        args = "xd";
                        break;
                    case 0x000A:
                        opcode = "KEY";
                        args = "x";
                        break;
                    case 0x0015:
                        opcode = "LD";
                        args = "dx";
                        break;
                    case 0x0018:
                        opcode = "LD";
                        args = "sx";
                        break;
                    case 0x001E:
                        opcode = "ADD";
                        args = "Ix";
                        break;
                    case 0x0029:
                        opcode = "FNT";
                        args = "x";
                        break;
                    case 0x0033:
                        opcode = "BCD";
                        args = "x";
                        break;
                    case 0x0055:
                        opcode = "LD";
                        args = "[x";
                        break;
                    case 0x0065:
                        opcode = "LD";
                        args = "x[";
                        break;
                }
                break;
            default:
                // TODO;
                break;
        }

        std::stringstream instr_ss;
        instr_ss << std::uppercase << opcode;
        if (args.length() != 0) instr_ss << " ";
        for (int i = 0; i < args.length(); i++) {
            if (args[i] == 'n')
                instr_ss << "0x" << std::hex << (instr & 0x0FFF) << std::dec;
            else if (args[i] == 'x' || args[i] == 'y')
                instr_ss << "V" << std::hex << ((instr & 0x0F00) >> 8) << std::dec;
            else if (args[i] == 'k')
                instr_ss << "0x" << std::hex << (instr & 0x00FF) << std::dec;
            else if (args[i] == 'I')
                instr_ss << "I";
            else if (args[i] == '0')
                instr_ss << "V0";
            else if (args[i] == 'N')
                instr_ss << "0x" << std::hex << (instr & 0x000F) << std::dec;
            else if (args[i] == 'd')
                instr_ss << "DT";
            else if (args[i] == 's')
                instr_ss << "ST";
            else if (args[i] == '[')
                instr_ss << "[I]";

            if (i != args.length()-1) instr_ss << ", ";
        }

        // std::cout << instr_ss.str() << '\n';
        instructions.push_back(instr_ss.str());
    }
}