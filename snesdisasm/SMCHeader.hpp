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

#ifndef SMCHEADER_HPP
#define SMCHEADER_HPP

#include <memory>
#include <cstdint>

class SMCHeader{
private:
    uint8_t *m_bytes;
public:
    SMCHeader() : m_bytes(nullptr) {}

    void load(uint8_t *headerData){
        m_bytes = headerData;
    }

    bool isLoROM(){
        return m_bytes[2] == 0x00;
    }

    operator bool() const { return m_bytes != nullptr; }
};

#endif // SMCHEADER_HPP
