#ifndef ROMADDRESS_HPP
#define ROMADDRESS_HPP

//romhack.wikia.com/wiki/SNES_ROM_layout

#include <cstdint>
#include "globals.h"
#include "Helper.hpp"

enum class ROM_LAYOUT {
    LOROM,
    HIROM,
    EXLOROM,
    EXHIROM
};

STRONG_TYPEDEF(uint32_t, ImageAddress)

class ROMAddress {
private:
    ROM_LAYOUT m_ROMLayout;

    uint32_t m_Address;
public:
    /*!
     * \brief Default constructs a LoROM address pointing at 0x0
     */
    ROMAddress();

    /*!
     * \brief ROMAddress creates a ROM default ROM address at 00:0000.
     * \param isLoROM must be true if the address calculations should be like in a LoROM. It's seen as a HiROM otherwise.
     */
    explicit ROMAddress(ROM_LAYOUT layout);

    /*!
     * \brief Construct the ROMAddress from a given Image Address
     * \param imageAddress
     */
    explicit ROMAddress(ROM_LAYOUT layout, ImageAddress imageAddress);

    /*!
     * \brief ROMAddress creates a ROM default ROM address at 00:0000.
     * \param isLoROM must be true if the address calculations should be like in a LoROM. It's seen as a HiROM otherwise.
       \param bankID the ID of the bank.
       \param bankAddress the address in a bank.
     */
    ROMAddress(bool isLoROM, uint8_t bankID, uint16_t bankAddress);

    /*!
      * \brief a destructor with no specific function besides deallocation.
      */
    ~ROMAddress();

    /*!
     * \brief Converts to an int
     */
    operator uint32_t() const;

    /*!
     * \brief adds a number of bytes to the address
     */
    void operator+=(int);

    /*!
     * \brief getImageAddress returns the offset from the beginning of the image data without the SMC header according to the membervariables.
     * \return the address in the image ignoring the SMC header.
     */
    ImageAddress getImageAddress() const;

    /*!
     * \brief sets the member variables so that they match the given address.
     * \param longAddress is the address the object will contain. It is supposed to be a 24 bit integer and an error will occur if it is greater than the max. value a 24 bit integer could contain.
     */
    void setROMAddress(uint32_t longAddress);

    /*!
     * \brief sets the ROM bank for the object.
     * \param ROMBank is the ID of the ROM bank.
     */
    void setROMBank(uint8_t m_ROMBank);

    /*!
     * \brief sets the address for data in a specific ROM bank.
     * \param addressInBank is the address in a specific bank.
     */
    void setInBankAddress(uint16_t addressInBank);

    /*!
     * \brief sets the ID of a page from a given parameter.
     * \param pageID the new pageID.
     */
    void setPageID(uint8_t pageID);

    /*!
     * \brief sets the address for data in a specific page.
     * \param addressInPage is the address in a specific page.
     */
    void setInPageAddress(uint8_t addressInPage);

    /*!
     * \brief calculates the ROM address from a given image offset.
     * \param imageOffset is the offset in the image from which the class evaluates it's ROM address.
     */
    void setROMAddressWithImageOffset(ImageAddress imageOffset);

    uint8_t bank() const { return (systemIsLittleEndian ? m_Address & 0xFF00 >> 8 : m_Address & 0xFF0000) >> 16; }
    uint16_t bankAddress() const { return systemIsLittleEndian ? m_Address & 0xFFFF0000 >> 16 : m_Address & 0x00FFFF; }
    uint8_t page() const { return (systemIsLittleEndian ? m_Address & 0xFF0000 >> 16 : m_Address & 0x00FF00) >> 8; }
    uint8_t pageAddress() const { return systemIsLittleEndian ? m_Address & 0xFF000000 >> 24 : m_Address & 0x0000FF; }

    /*!
     * \brief operator<< prints the address in big-endian with colon between the bank and the address in the bank.
     * \param stream is the text outputstream where the address gets printed.
     * \param addr is the ROMAddress instance the function will print.
     * \return the reference of the stream.
     */
    friend std::ostream& operator<<(std::ostream &stream, const ROMAddress &addr);
};

#endif // ROMADDRESS_HPP
