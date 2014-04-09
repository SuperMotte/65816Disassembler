#include "ROMAddress.hpp"
#include <assert.h>
#include <iostream>
#include <iomanip>

ROMAddress::ROMAddress()
    : ROMAddress(ROM_LAYOUT::LOROM)
{

}

ROMAddress::ROMAddress(ROM_LAYOUT layout)
    : m_ROMLayout(layout), m_Address(0)
{
}

ROMAddress::ROMAddress(ROM_LAYOUT layout, ImageAddress imageAddress)
    : ROMAddress(layout)
{
    setROMAddressWithImageOffset(imageAddress);
}

ROMAddress::ROMAddress(bool isLoROM, uint8_t bankID, uint16_t bankAddress)
    : m_ROMLayout(isLoROM ? ROM_LAYOUT::LOROM : ROM_LAYOUT::HIROM),
      m_Address((bankID << 16) | bankAddress){
}

ROMAddress::~ROMAddress() {
}

void ROMAddress::operator+=(int i)
{
    m_Address += i;
}

ROMAddress::operator uint32_t() const
{
    return m_Address;
}

ImageAddress ROMAddress::getImageAddress() const {
    uint32_t imageAddress = 0;
    uint8_t bank = this->bank();

    if(m_ROMLayout == ROM_LAYOUT::LOROM) {

        if(bank == 0x7E || bank == 0x7F) {
            return ImageAddress(-1); //the address is actually a RAM address
        } else {
            //!TODO: save-RAM (?)
            //it is actually a ROM address
            imageAddress = (bank & 0x7F)*0x10000/2+bankAddress()&0x7FFF;
        }
    } else {
        //it is a HiROM address
        if(bank == 0x7E || bank == 0x7F) {
            return ImageAddress(-1); //the address is actually a RAM address
        } else {
            //!TODO: save-RAM (?)
            //it is actually a ROM address
            if((bank & 0x7F) < 0x40) {
                //mirror 3 and 4
                assert(bankAddress() > 0x7FFF);
                imageAddress = ((bank & 0x7F) + 0xC0)*0x10000+bankAddress();
            } else {
                //mirror 1 and 2
                imageAddress = ((bank & 0x7F) - 0x40)*0x10000+bankAddress();
            }
        }
    }

    return ImageAddress(imageAddress);
}

void ROMAddress::setROMAddress(uint32_t longAddress) {
    assert(longAddress < 0x1000000);
    m_Address = longAddress;
}

void ROMAddress::setROMBank(uint8_t ROMBank) {
    m_Address = (ROMBank << 16) | (m_Address & 0x00ffff);
}

void ROMAddress::setInBankAddress(uint16_t addressInBank) {
    m_Address = (m_Address & 0xff0000) | addressInBank;
}

void ROMAddress::setPageID(uint8_t pageID) {
    m_Address = (m_Address & 0xff00ff) | (pageID << 8);
}

void ROMAddress::setInPageAddress(uint8_t addressInPage) {
    m_Address = (m_Address & 0xffff00) | addressInPage;
}

//romhack.wikia.com/wiki/SNES_ROM_layout
void ROMAddress::setROMAddressWithImageOffset(ImageAddress imageOffset) {
    assert(imageOffset < 0x400000);

    switch(m_ROMLayout){
    case ROM_LAYOUT::LOROM:
    {
        m_Address = static_cast<uint32_t>(imageOffset);

        uint8_t bank = imageOffset / 0x8000;
        uint16_t bankAddress = 0x8000 + (imageOffset & 0x7FFF);
        if(imageOffset > 0x3DFFFF) {
            //we have to use the second mirror because otherwise we would refer to system RAM
            bank += 0x80;
        }
        m_Address = (bank << 16) | bankAddress;
    }
    break;
    case ROM_LAYOUT::HIROM:
    {
        //it is a HiROM

        uint8_t bank = imageOffset / 0x10000;
        uint16_t bankAddress = imageOffset % 0xFFFF;
        if(imageOffset > 0x3DFFFF) {
            //we have to use the second mirror because otherwise we would refer to system RAM
            bank += 0x80;
        }
        m_Address = (bank << 16) | bankAddress;
    }
    break;
    default: //!TODO: throw some errors
        break;
    }
}

std::ostream& operator<<(std::ostream &stream, const ROMAddress &addr) {
    auto flags = stream.flags(std::ios_base::hex);
    //so the address gets shown as big-endian no matter if the system is little-, middle- or big-endian
    stream << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(addr.bank()) << ":" << std::setw(4) << addr.bankAddress();
    stream.flags(flags);
    return stream;
}
