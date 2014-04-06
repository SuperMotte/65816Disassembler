#ifndef ROMADDRESS_HPP
#define ROMADDRESS_HPP

#include <cstdint>

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

    uint8_t m_ROMBank;
    union {
        struct {
            //stupid little-endian, we had to exchange the order of these two
            uint8_t m_pageOffset;
            uint8_t m_pageNumber;
        };
        struct {
            uint16_t m_bankAddress;
        };
    };

public:
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
      * \brief a destructor with no specific function besides deallocation.
      */
    ~ROMAddress();

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
};

#endif // ROMADDRESS_HPP
