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

#ifndef DISASM_HPP
#define DISASM_HPP

#include "SNESROM.hpp"
#include "Instructions.hpp"
#include "CPUState.hpp"

#include <vector>
#include <memory>

/*! \brief The disassembler class.
 *
 *  This class consumes a \see SNESROM and offers the interface to generate sequences of
 *  instructions from the binary data.
 */
class Disasm
{    
public:
    /*! \brief A sequence of instructions
     *
     *  \todo Add better interface to this type. It is still possible to invalidate the invarance if start and
     *        end to point at the correct bytes.
     */
    struct Section{
        /*! \brief The first byte of the first instruction within the sequence
         */
        ROMAddress start;

        /*! \brief The first byte after the last instruction within the sequence
         */
        ROMAddress end;
        
        /*! \brief The vector of instructions contained within the sequence
         */
        std::vector<Instruction> instructions;
    };
private:
    SNESROM m_Rom;
    CPUState m_State;
public:
    /*! \brief Constructs disassembler. This constructor will take ownership of the given rom.
     *  \param rom the rom to disassemble
     */
    Disasm(SNESROM && rom);

    /*! \brief Returns a reference to the stored rom.
     *
     * \todo this might be a leak of internal state. we should better provide the needed interface directly in Disasm
     */
    const SNESROM& rom();
    
    /*! \brief Constructs a \see Section starting at start
     *
     *  This method will fetch consectuive instructions beginning at start until it reaches a maximum instruction limit
     *  or until it encounters a jump instruction (branches, conditional jumps and the like).
     *
     *  \param start the address to start disassembling at
     *  \param max_instructions the maximum number of instructions to fetch
     */
    Section disasmUntilJump(ROMAddress start, unsigned int max_instructions = 30) const;
};

#endif // DISASM_HPP
