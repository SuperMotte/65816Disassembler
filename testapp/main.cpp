#include <iostream>

#include "snesdisasm/snesdisasmConfig.hpp"
#include "snesdisasm/Logger.hpp"
#include "snesdisasm/SNESROM.hpp"
#include "snesdisasm/Disasm.h"
using namespace std;

int main() {
    cout << "Testapp using libsnesdisasm version: " << snesdisasm::version_string << endl;

    //setup the disassembler from the rom
    Disasm disasm(SNESROM("TERRA.SMC"));

    //get the roms entry point
    ROMAddress pos = disasm.rom().header().getInterruptDest(EmulationIV::RESET());

    //disassemble until you hit a jump instruction
    Disasm::Section section = disasm.disasmUntilJump(pos);

    //print out all found instructions
    for(const Instruction &ins : section.instructions) {
        std::cout << std::hex << pos << ": \t" << ins.stringify() << std::endl;
        pos += ins.size();
    }

    return 0;
}

