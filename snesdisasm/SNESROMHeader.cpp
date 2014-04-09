#include "SNESROMHeader.hpp"
#include <assert.h>
#include <cstring> //memset, and the like

SNESROMHeader::SNESROMHeader()
    : m_HeaderData(nullptr) {

}

SNESROMHeader::SNESROMHeader(uint8_t *headerData)
    : m_HeaderData(headerData) {

    assert(isThere(headerData));
}

bool SNESROMHeader::isThere(const uint8_t *headerData)
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
}

std::string SNESROMHeader::getROMName() const {
    return std::string(m_HeaderData + m_ROMNameIndex, m_HeaderData + m_ROMNameIndex + m_ROMNameLength);
}

bool SNESROMHeader::isLoROM() const {
    return (m_HeaderData[m_ROMLayoutIndex] & 0x01) != 0x01;
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

SNESROMHeader::Address SNESROMHeader::getInterruptDest(NativeIV vector) const {
    SNESROMHeader::Address addr = 0;
    addr = (m_HeaderData[m_NativeInterruptVectorIndex + 2*static_cast<unsigned int>(vector) + 1] << 8)
         | m_HeaderData[m_NativeInterruptVectorIndex + 2*static_cast<unsigned int>(vector)];
    return addr;
    return 0;
}

SNESROMHeader::Address SNESROMHeader::getInterruptDest(EmulationIV vector) const {
    SNESROMHeader::Address addr = 0;
    addr = (m_HeaderData[m_EmulationInterruptVectorIndex + 2*static_cast<unsigned int>(vector) + 1] << 8)
         | m_HeaderData[m_EmulationInterruptVectorIndex + 2*static_cast<unsigned int>(vector)];
    return addr;
}
