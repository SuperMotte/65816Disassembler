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

/*! \brief SMCHeader is a wrapper around a non-owning pointer to the rom data containing the SMC header
 *
 * It adds an interface to interact with the SMC header
 */
class SMCHeader{
private:
    /*! \brief m_bytes contains the pointer to the address where the SMC header starts and nullptr otherwise
     */
    const uint8_t *m_bytes;
public:
    /*! \brief constructs an SMC header in its invalid state
     */
    SMCHeader() : m_bytes(nullptr) {}

    /*! \brief constructs an SMC header
     *
     * If start is nullptr, the SMC header is constructed in its invalid state. Otherwise it assumes that start
     * points to the first byte of the SMC header. This is semantically equivalent to construct an invalid header
     * and call \see load with the start address.
     */
    SMCHeader(const uint8_t *start) : SMCHeader() {load(start);}

    /*! \brief Loads the smc header from a given address
     *
     * \param headerData the address to load the header data from
     */
    void load(const uint8_t *headerData){
        m_bytes = headerData;
    }

    /*! \brief Returns true if the SMC header specifies the ROM to be a LoROM image
     *
     * Be aware that this information is sometimes incorrect.
     */
    bool isLoROM(){
        return m_bytes[2] == 0x00;
    }

    /*! \brief Converts to true if the smc header is not in its invalid state and false otherwise.
     */
    operator bool() const { return m_bytes != nullptr; }
};

#endif // SMCHEADER_HPP
