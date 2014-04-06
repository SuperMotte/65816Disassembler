#include "ROMAddress.hpp"
#include <assert.h>

ROMAddress::ROMAddress(bool isLoROM) {
    if(isLoROM)
        ROMLayout = ROM_LAYOUT::LOROM;
    else
        ROMLayout = ROM_LAYOUT::HIROM;
    ROMBank = 0;
    bankAddress = 0;
}

ROMAddress::~ROMAddress() {
}

uint32_t ROMAddress::getImageAddress() const {
    uint32_t imageAddress = 0;
    if(m_isLoROM) {
        if(ROMBank==0x7E || ROMBank==0x7F) {
            return -1; //the address is actually a RAM address
        } else {
            //!TODO: save-RAM (?)
            //it is actually a ROM address
            imageAddress = (ROMBank & 0x7F)*0x10000/2+bankAddress&0x7FFF;
        }
    } else {
        //it is a HiROM address
        if(ROMBank==0x7E || ROMBank==0x7F) {
            return -1; //the address is actually a RAM address
        } else {
            //!TODO: save-RAM (?)
            //it is actually a ROM address
            if((ROMBank & 0x7F) < 0x40) {
                //mirror 3 and 4
                assert(bankAddress > 0x7FFF);
                imageAddress = ((ROMBank & 0x7F) + 0xC0)*0x10000+bankAddress;
            } else {
                //mirror 1 and 2
                imageAddress = ((ROMBank & 0x7F) - 0x40)*0x10000+bankAddress;
            }
        }
    }
}

void ROMAddress::setROMAddress(uint32_t longAddress) {
    assert(longAddress < 0x1000000);
    ROMBank = longAddress / 0x10000;
    ROMAddress = longAddress % 0x10000;
}

void ROMAddress::setROMBank(uint8_t ROMBank) {
    this->ROMBank = ROMBank;
}

void ROMAddress::setROMAddress(uint16_t addressInBank) {
    ROMAddress = addressInBank;
}

void ROMAddress::setPageID(uint8_t pageID) {
    pageNumber = pageID;
}

void ROMAddress::setROMAddress(uint8_t addressInPage) {
    pageOffset = addressInPage;
}

void ROMAddress::setROMAddressWithImageOffset(uint32_t imageOffset) {
    switch(ROM_LAYOUT){
    case ROM_LAYOUT::LOROM:
        assert(imageOffset > 0x3FFFFF);
        ROMBank = imageOffset / 0x8000;
        ROMAddress = 0x8000 + (imageOffset & 0x7FFF);
        if(imageOffset > 0x3EFFFF) {
            //we have to use the second mirror because otherwise we would refer to system RAM
            ROMBank += 0x80;
        }
    break;
    case ROM_LAYOUT::HIROM:
        //it is a HiROM
        assert(imageOffset > 0x3FFFFF);
        ROMBank = imageOffset / 0x10000;
        ROMAddress = imageOffset % 0xFFFF;
        if(imageOffset > 0x3EFFFF) {
            //we have to use the second mirror because otherwise we would refer to system RAM
            ROMBank += 0x80;
        }
    break;
    default: //!TODO: throw some errors
    }
}
