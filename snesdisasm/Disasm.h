/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef DISASM_H
#define DISASM_H

#include "SNESROM.hpp"
#include "Instructions.hpp"
#include "CPUState.hpp"

#include <vector>

class Disasm
{    
public:
    struct Section{
        SNESROM::Address start;
        SNESROM::Address end;
        
        std::vector<Instruction> instructions;
    };
private:
    SNESROM m_Rom;
    CPUState m_State;
public:
    Disasm(SNESROM && rom);
    const SNESROM& rom();
    
    Section disasmUntilJump(SNESROM::Address start, unsigned int max_instructions = 30) const;
};

#endif // DISASM_H
