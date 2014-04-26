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

#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "MachineState.hpp"

#include <cstdint>
#include <string>

/*! \brief A fetched instruction
 *
 *  This class contains a fetched instruction including its arguments.
 *
 *  \todo maybe it is sufficient to store a pointer to the instruction in memory and its size.
 *        The lifetime of each instruction would then depend on the lifetime of the ROM. An it
 *        would be impossible to copy a rom since it would invalidate all those pointers.
 */
class Instruction {
  public:
    typedef uint16_t Address;

    //be aware that arguments are in litte-endian and should be converted whenn passing
    //them through the public interface of Instruction
    //!TODO Think about if this really has to be public :?
    struct Argument_t {
        uint8_t at1;
        uint8_t at2;
        uint8_t at3;

        Argument_t(): at1(0), at2(0), at3(0) {}
    };

  private:
    uint8_t m_OpCode;
    Argument_t m_Argument;
    uint8_t m_Size;
  public:
    /*! \brief Fetches a instruction from the bytes pointed at by data. It uses the given \see CPUState.
     */
    explicit Instruction(const MachineState &state, const uint8_t *data);

    /*! \brief Returns the size of the Instruction in bytes. This includes the arguments.
     *
     * Thus, the next instruction starts Instruction::size() bytes after this one in ROM memory.
     */
    uint8_t size() const;

    /*! \brief Returns true if the instruction is a jump i.e. if the next instruction to
     *         execute may not be the next instruction in ROM memory.
     *
     *  This includes (unconditional) jumps, calls, returns and branches.
     */
    bool isJump() const;

    /*! \brief Returns a string representation if the instruction
     *
     *  This method is mainly for debugging purpose and the string representation of each instruction
     *  may be subject to heavy modifications during developmen. It is therefore not suitable for parsing.
     */
    std::string stringify() const;
};

#endif // INSTRUCTIONS_H
