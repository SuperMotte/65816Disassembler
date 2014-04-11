#include "SNESROMHeader.hpp"
#include <assert.h>
#include <cstring> //memset, and the like

SNESROMHeader::SNESROMHeader()
    : m_HeaderData(nullptr) {

}

SNESROMHeader::SNESROMHeader(const uint8_t *headerData)
    : m_HeaderData(headerData) {

    assert(mayBeThere(headerData));
}

bool SNESROMHeader::mayBeThere(const uint8_t *headerData)
{
    //bit 0x20 is always set in the ROM layout
    //country codes ranging from 0x0E to 0xFF are invalid
    return (headerData[m_ROMLayoutIndex] & 0x20) &&
           (headerData[m_CountryCodeIndex] < 0x0E);
}

SNESROMHeader::SNESROMHeader(SNESROMHeader &&other) noexcept
    : m_HeaderData(other.m_HeaderData) {
    other.m_HeaderData = nullptr;
}

SNESROMHeader::~SNESROMHeader() {
}

const SNESROMHeader &SNESROMHeader::operator=(SNESROMHeader && other) noexcept{
    m_HeaderData = other.m_HeaderData;
    other.m_HeaderData = nullptr;
    return *this;
}

std::string SNESROMHeader::getROMName() const {
    return std::string(m_HeaderData + m_ROMNameIndex, m_HeaderData + m_ROMNameIndex + m_ROMNameLength);
}

RomLayout SNESROMHeader::layout() const {
    return RomLayout::fromByteCode(m_HeaderData[m_ROMLayoutIndex]);
}

bool SNESROMHeader::isFastROM() const {
    return (m_HeaderData[m_ROMLayoutIndex] & 0x10);
}

SNESROMHeader::size_type SNESROMHeader::getROMSize() const {
    SNESROMHeader::size_type size = 1024;

    for(uint8_t i = 0; i < m_HeaderData[m_ROMSizeIndex]; ++i) {
        size *= 2;
    }

    return size;
}

int SNESROMHeader::getRAMSize() const {
    if(m_HeaderData[m_ROMSizeIndex] == 0) {
        return 0;
    } else {
        size_type ROMSize = 2048;
        for(uint8_t i = 1; i <  m_HeaderData[m_ROMSizeIndex]; ++i) {
            ROMSize *= 2;
        }
        return ROMSize;
    }
}

int SNESROMHeader::getColorTransmissionSystem() const {
    if(m_HeaderData[m_CountryCodeIndex] <= 0x01) {
        return NTSC;
    } else if(m_HeaderData[m_CountryCodeIndex] <= 0x0C) {
        return PAL;
    } else {
        return NTSC;
    }
}

ROMAddress SNESROMHeader::getInterruptDest(NativeIV vector) const {
    uint16_t addr = (m_HeaderData[m_NativeInterruptVectorIndex + 2*static_cast<unsigned int>(vector) + 1] << 8)
                  | m_HeaderData[m_NativeInterruptVectorIndex + 2*static_cast<unsigned int>(vector)];

    ROMAddress result = getROMAddressObject(layout());
    result.setROMAddress(addr);
    return result;
}

ROMAddress SNESROMHeader::getInterruptDest(EmulationIV vector) const {
    uint16_t addr = (m_HeaderData[m_EmulationInterruptVectorIndex + 2*static_cast<unsigned int>(vector) + 1] << 8)
                  | m_HeaderData[m_EmulationInterruptVectorIndex + 2*static_cast<unsigned int>(vector)];

    ROMAddress result = getROMAddressObject(layout());
    result.setROMAddress(addr);
    return result;
}
