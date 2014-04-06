#include "ROMAddress.hpp"
#include <assert.h>

ROMAddress::ROMAddress(ROM_LAYOUT layout)
    : m_ROMLayout(layout), m_ROMBank(0), m_bankAddress(0)
{
}

ROMAddress::ROMAddress(ROM_LAYOUT layout, ImageAddress imageAddress)
    : ROMAddress(layout)
{
    setROMAddressWithImageOffset(imageAddress);
}

ROMAddress::~ROMAddress() {
}

ImageAddress ROMAddress::getImageAddress() const {
    uint32_t imageAddress = 0;
    if(m_ROMLayout == ROM_LAYOUT::LOROM) {
        if(m_ROMBank==0x7E || m_ROMBank==0x7F) {
            return ImageAddress(-1); //the address is actually a RAM address
        } else {
            //!TODO: save-RAM (?)
            //it is actually a ROM address
            imageAddress = (m_ROMBank & 0x7F)*0x10000/2+m_bankAddress&0x7FFF;
        }
    } else {
        //it is a HiROM address
        if(m_ROMBank==0x7E || m_ROMBank==0x7F) {
            return ImageAddress(-1); //the address is actually a RAM address
        } else {
            //!TODO: save-RAM (?)
            //it is actually a ROM address
            if((m_ROMBank & 0x7F) < 0x40) {
                //mirror 3 and 4
                assert(m_bankAddress > 0x7FFF);
                imageAddress = ((m_ROMBank & 0x7F) + 0xC0)*0x10000+m_bankAddress;
            } else {
                //mirror 1 and 2
                imageAddress = ((m_ROMBank & 0x7F) - 0x40)*0x10000+m_bankAddress;
            }
        }
    }
}

void ROMAddress::setROMAddress(uint32_t longAddress) {
    assert(longAddress < 0x1000000);
    m_ROMBank = longAddress / 0x10000;
    m_bankAddress = longAddress % 0x10000;
}

void ROMAddress::setROMBank(uint8_t ROMBank) {
    m_ROMBank = ROMBank;
}

void ROMAddress::setInBankAddress(uint16_t addressInBank) {
    m_bankAddress = addressInBank;
}

void ROMAddress::setPageID(uint8_t pageID) {
    m_pageNumber = pageID;
}

void ROMAddress::setInPageAddress(uint8_t addressInPage) {
    m_pageOffset = addressInPage;
}

void ROMAddress::setROMAddressWithImageOffset(ImageAddress imageOffset) {
    switch(m_ROMLayout){
    case ROM_LAYOUT::LOROM:
        assert(imageOffset > 0x3FFFFF);
        m_ROMBank = imageOffset / 0x8000;
        m_bankAddress = 0x8000 + (imageOffset & 0x7FFF);
        if(imageOffset > 0x3EFFFF) {
            //we have to use the second mirror because otherwise we would refer to system RAM
            m_ROMBank += 0x80;
        }
    break;
    case ROM_LAYOUT::HIROM:
        //it is a HiROM
        assert(imageOffset > 0x3FFFFF);
        m_ROMBank = imageOffset / 0x10000;
        m_bankAddress = imageOffset % 0xFFFF;
        if(imageOffset > 0x3EFFFF) {
            //we have to use the second mirror because otherwise we would refer to system RAM
            m_ROMBank += 0x80;
        }
    break;
    default: //!TODO: throw some errors
        break;
    }
}
