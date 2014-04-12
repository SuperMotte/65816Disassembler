#include "ROMAddress.hpp"
#include <assert.h>
#include <iostream>
#include <iomanip>

std::ostream& operator<<(std::ostream &stream, const RomLayout &layout)
{
    switch(layout.m_layout){
    case 0:
        stream << "LoROM";
        break;
    case 1:
        stream << "HiROM";
        break;
    case 2:
        stream << "ExLoROM (unsupported)";
        break;
    case 3:
        stream << "ExHiROM (unsupported)";
        break;
    default: stream << "Error";
    }

    return stream;
}

ROMAddress::ROMAddress(uint8_t bankID, uint16_t bankAddress)
    : m_Address((bankID << 16) | bankAddress)
{

}

ROMAddress::ROMAddress()
    : m_Address(0x00) //this is important for a non-debug-build
{

}

void ROMAddress::operator+=(int i)
{
    ///\todo this is unsafe. we may end up in wrong spaces
    m_Address += i;
}

void ROMAddress::setROMAddress(uint32_t longAddress) {
    assert(longAddress < 0x1000000);
    m_Address = longAddress;
}

void ROMAddress::setROMBank(uint8_t ROMBank) {
    m_Address = (ROMBank << 16) | (m_Address & 0x00FFFF);
}

void ROMAddress::setBankAddress(uint16_t addressInBank) {
    m_Address = (m_Address & 0xFF0000) | addressInBank;
}

void ROMAddress::setPageID(uint8_t pageID) {
    m_Address = (m_Address & 0xFF00FF) | (pageID << 8);
}

void ROMAddress::setPageAddress(uint8_t addressInPage) {
    m_Address = (m_Address & 0xFFFF00) | addressInPage;
}

std::ostream& operator<<(std::ostream &stream, const ROMAddress &addr) {
    auto flags = stream.flags(std::ios_base::hex);
    //so the address gets shown as big-endian no matter if the system is little-, middle- or big-endian
    stream << std::setfill('0') << std::uppercase << std::setw(2) << static_cast<uint16_t>(addr.bank()) << ":" << std::setw(4) << addr.bankAddress();
    stream.flags(flags);
    return stream;
}

LoROMAddress::LoROMAddress()
    : ROMAddress()
{

}

LoROMAddress::LoROMAddress(ImageAddress imageAddress)
{
    fromImageAddress(imageAddress);
}

LoROMAddress::LoROMAddress(uint8_t bankID, uint16_t bankAddress)
    : ROMAddress(bankID, bankAddress)
{

}

//romhack.wikia.com/wiki/SNES_ROM_layout
ImageAddress LoROMAddress::toImageAddress() const {
    /// \todo: save-RAM (?)

    uint32_t imageAddress = 0;
    uint8_t bank = this->bank();

    if(bank == 0x7E || bank == 0x7F) {
        return ImageAddress(-1); //the address is actually a RAM address
    } else {
        //it is actually a ROM address
        imageAddress = (bank & 0x7F)*0x10000/2+bankAddress()&0x7FFF;
    }

    return ImageAddress(imageAddress);
}

//romhack.wikia.com/wiki/SNES_ROM_layout
void LoROMAddress::fromImageAddress(ImageAddress imageAdress) {
    assert(imageAdress < 0x400000);

    uint8_t bank = imageAdress / 0x8000;
    uint16_t bankAddress = 0x8000 + (imageAdress & 0x7FFF);
    if(imageAdress > 0x3DFFFF) {
        //we have to use the second mirror because otherwise we would refer to system RAM
        bank += 0x80;
    }
    m_Address = (bank << 16) | bankAddress;
}

HiROMAddress::HiROMAddress()
    : ROMAddress()
{

}

HiROMAddress::HiROMAddress(ImageAddress imageAddress)
{
    fromImageAddress(imageAddress);
}

HiROMAddress::HiROMAddress(uint8_t bankID, uint16_t bankAddress)
    : ROMAddress(bankID, bankAddress)
{

}

//romhack.wikia.com/wiki/SNES_ROM_layout
ImageAddress HiROMAddress::toImageAddress() const {
    /// \todo: save-RAM (?)

    uint32_t imageAddress = 0;
    uint8_t bank = this->bank();

    if(bank == 0x7E || bank == 0x7F) {
        return ImageAddress(-1); //the address is actually a RAM address
    } else {
        //!TODO: save-RAM (?)
        //it is actually a ROM address

        imageAddress = m_Address & 0x3FFFFF;
        if((bank & 0x7F) < 0x40) {
            //mirrors 3 and 4 are only accepted in the upper half
            assert(bankAddress() > 0x7FFF);
        }
        imageAddress = ((bank & 0x3F)<<16)+bankAddress();
    }

    return ImageAddress(imageAddress);
}

//romhack.wikia.com/wiki/SNES_ROM_layout
void HiROMAddress::fromImageAddress(ImageAddress imageAdress) {
    assert(imageAdress < 0x400000);

    uint8_t bank = imageAdress / 0x10000;
    uint16_t bankAddress = imageAdress % 0xFFFF;
    if(imageAdress > 0x3DFFFF) {
        //we have to use the second mirror because otherwise we would refer to system RAM
        bank += 0x80;
    }
    m_Address = (bank << 16) | bankAddress;
}

ROMAddress* getROMAddressObject(RomLayout layout) {
    if(layout==RomLayout::LoROM()) {
        return new LoROMAddress();
    }
    if(layout==RomLayout::HiROM()) {
        return new HiROMAddress();
    }
    //!ToDo Throw exceptions. EXCEPTIONS FOR EVERYONE!!!
}
