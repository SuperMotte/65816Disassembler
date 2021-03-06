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

#include "Disasm.hpp"

Disasm::Disasm(SNESROM  &&rom)
    : m_ROM(std::forward<SNESROM>(rom)) {
}

const SNESROM &Disasm::rom() {
    return m_ROM;
}

Disasm::Section Disasm::disasmUntilJump(ROMAddress* start, unsigned int maxInstructions) const {
    Section section;
    section.start = start;
    ROMAddress* pos = start;

    for(unsigned int i = 0; i < maxInstructions; ++i) {
        Instruction inst(m_State, m_ROM[pos]);
        section.instructions.push_back(inst);
        (*pos) += inst.size();

        if(inst.isJump()) {
            break;
        }
    }

    section.end = pos;

    return section;
}
