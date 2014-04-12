#ifndef ROMADDRESS_HPP
#define ROMADDRESS_HPP

//romhack.wikia.com/wiki/SNES_ROM_layout

#include <cstdint>
#include <iosfwd>

#include "Helper.hpp"

class RomLayout{
    uint8_t m_layout;
    explicit RomLayout(uint8_t layout): m_layout(layout) {}
public:
    static RomLayout LoROM(){ return RomLayout(0); }
    static RomLayout HiROM(){ return RomLayout(1); }
    static RomLayout ExLoROM(){ return RomLayout(2);} //this and
    static RomLayout ExHiROM(){ return RomLayout(3);} //this is currently not supported
    static RomLayout Error(){ return RomLayout(4);}

    bool isLoROM() const { return m_layout == 0; }
    static RomLayout fromByteCode(uint8_t code){ if((code & 0x01) != 0x01) return LoROM(); else return HiROM();}
    bool operator==(const RomLayout& other) const { return m_layout == other.m_layout; }

    friend std::ostream& operator<<(std::ostream &stream, const RomLayout &layout);
};

STRONG_TYPEDEF(uint32_t, ImageAddress)

class ROMAddress {
protected:
    uint32_t m_Address;
public:
    ROMAddress();
    ROMAddress(uint8_t bankID, uint16_t bankAddress);
    /*!
     * \brief adds a number of bytes to the address
     */
    void operator+=(int);

    /*!
     * \brief getImageAddress returns the offset from the beginning of the image data without the SMC header according to the membervariables.
     * \return the address in the image ignoring the SMC header.
     */
    virtual ImageAddress toImageAddress() const = 0;

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
    void setBankAddress(uint16_t addressInBank);

    /*!
     * \brief sets the ID of a page from a given parameter.
     * \param pageID the new pageID.
     */
    void setPageID(uint8_t pageID);

    /*!
     * \brief sets the address for data in a specific page.
     * \param addressInPage is the address in a specific page.
     */
    void setPageAddress(uint8_t addressInPage);

    /*!
     * \brief calculates the ROM address from a given image offset.
     * \param layout is the layout of the ROM, should be RomLayout::LoROM() or RomLayout::HiROM().
     * \param imageOffset is the offset in the image from which the class evaluates it's ROM address.
     */
    virtual void fromImageAddress(ImageAddress imageAddress) = 0;

    uint8_t bank() const { return (m_Address & 0xFF0000) >> 16; }
    uint16_t bankAddress() const { return m_Address & 0x00FFFF; }
    uint8_t page() const { return (m_Address & 0x00FF00) >> 8; }
    uint8_t pageAddress() const { return m_Address & 0x0000FF; }

    /*!
     * \brief operator<< prints the address in big-endian with colon between the bank and the address in the bank.
     * \param stream is the text outputstream where the address gets printed.
     * \param addr is the ROMAddress instance the function will print.
     * \return the reference of the stream.
     */
    friend std::ostream& operator<<(std::ostream &stream, const ROMAddress &addr);
};

class LoROMAddress : public ROMAddress {
public:
    /*!
     * \brief Default constructs an address pointing at 00:0000
     */
    LoROMAddress();
    /*!
     * \brief Construct the LoROMAddress from a given Image Address.
     * \param imageAddress is the address in an image the object will point to.
     */
    explicit LoROMAddress(ImageAddress imageAddress);
    /*!
     * \brief LoROMAddress creates an address object at bankID:bankAddress.
     * \param bankID the ID of the bank.
     * \param bankAddress the address in a bank.
     */
    LoROMAddress(uint8_t bankID, uint16_t bankAddress);
    virtual ImageAddress toImageAddress() const override;
    virtual void fromImageAddress(ImageAddress imageAddress) override;
};

class HiROMAddress : public ROMAddress {
public:
    /*!
     * \brief Default constructs an address pointing at 00:0000
     */
    HiROMAddress();
    /*!
     * \brief Construct the HiROMAddress from a given Image Address.
     * \param imageAddress is the address in an image the object will point to.
     */
    explicit HiROMAddress(ImageAddress imageAddress);
    /*!
     * \brief HiROMAddress creates an address object at bankID:bankAddress.
     * \param bankID the ID of the bank.
     * \param bankAddress the address in a bank.
     */
    HiROMAddress(uint8_t bankID, uint16_t bankAddress);
    virtual ImageAddress toImageAddress() const override;
    virtual void fromImageAddress(ImageAddress imageAddress) override;
};


ROMAddress* getROMAddressObject(RomLayout layout);

#endif // ROMADDRESS_HPP
