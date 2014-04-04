#include <iostream>

#include "snesdisasm/snesdisasmConfig.hpp"
#include "snesdisasm/Logger.hpp"
#include "snesdisasm/SNESROM.hpp"
#include "snesdisasm/Disasm.h"

using namespace std;

int main() {
    cout << "Testapp using libsnesdisasm version: " << snesdisasm::version_string << endl;
    
    SNESROM TerranigmaROM("TERRA.SMC");
    //TerranigmaROM.overwriteLoROMHiROM(false); //"TERRANIGMA D" has probably a bad SMC-header, overwrite it so it is handled as a HiROM*/

    Disasm disasm(&TerranigmaROM);
    auto pos = TerranigmaROM.header().getInterruptDest(EmulationIV::RESET());
    Disasm::Section section = disasm.disasmUntilJump(pos);
    
    for(const Instruction& ins : section.mInstructions) {
        std::cout << std::hex << pos << ": \t" << ins.stringify() << std::endl;
        pos += ins.bytes();
    }
    
    return 0;
}

