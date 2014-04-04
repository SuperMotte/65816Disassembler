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

#include "CPUState.hpp"

#include <cstdint>
#include <string>

class Instruction {
  public:
    typedef uint16_t Address;

    union Argument_t {
        struct {
            uint8_t at1;
            uint8_t at2;
            uint8_t at3;
        } as8;

        struct {
            uint16_t at1;
            uint8_t at2;
        } as16_8;
    };

  private:
    uint8_t m_OpCode;
    Argument_t m_Argument;
    uint8_t m_Size;
  public:
    explicit Instruction(const CPUState &state, uint8_t *data);

    uint8_t size() const;

    bool isJump() const;

    std::string stringify() const;
};

#endif // INSTRUCTIONS_H
