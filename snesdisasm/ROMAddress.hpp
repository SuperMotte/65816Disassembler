#ifndef ROMADDRESS_HPP
#define ROMADDRESS_HPP

#include <cstdint>

enum class ROM_LAYOUT {
    LOROM,
    HIROM,
    EXLOROM,
    EXHIROM
};

class ROMAddress {
private:
    ROM_LAYOUT ROMLayout;

    uint8_t ROMBank;
    union {
        struct {
            //stupid little-endian, we had to exchange the order of these two
            uint8_t pageOffset;
            uint8_t pageNumber;
        };
        struct {
            uint16_t bankAddress;
        };
    };

public:
    /*!
     * \brief ROMAddress creates a ROM default ROM address at 00:0000.
     * \param isLoROM must be true if the address calculations should be like in a LoROM. It's seen as a HiROM otherwise.
     */
    ROMAddress(bool isLoROM);
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
     * \brief getImageAddress returns the offset from the beginning of the image data without the SMC header according to the membervariables.
     * \return the address in the image ignoring the SMC header.
     */
    uint32_t getImageAddress() const;
    /*!
     * \brief setROMAddress sets the member variables so that they match the given address.
     * \param longAddress is the address the object will contain. It is supposed to be a 24 bit integer and an error will occur if it is greater than the max. value a 24 bit integer could contain.
     */
    void setROMAddress(uint32_t longAddress);
    /*!
     * \brief setROMBank sets the ROM bank for the object.
     * \param ROMBank is the ID of the ROM bank.
     */
    void setROMBank(uint8_t ROMBank);
    /*!
     * \brief setROMAddress sets the address for data in a specific ROM bank.
     * \param addressInBank is the address in a specific bank.
     */
    void setROMAddress(uint16_t addressInBank);
    /*!
     * \brief setPageID sets the ID of a page from a given parameter.
     * \param pageID the new pageID.
     */
    void setPageID(uint8_t pageID);
    /*!
     * \brief setROMAddress sets the address for data in a specific page.
     * \param addressInPage is the address in a specific page.
     */
    void setROMAddress(uint8_t addressInPage);
    /*!
     * \brief setROMAddressWithImageOffset calculates the ROM address from a given image offset.
     * \param imageOffset is the offset in the image from which the class evaluates it's ROM address.
     */
    void setROMAddressWithImageOffset(uint32_t imageOffset);
};

#endif // ROMADDRESS_HPP
