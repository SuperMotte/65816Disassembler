#include "SNESROMHeader.hpp"
#include <assert.h>
#include <cstring> //memset, and the like

SNESROMHeader::SNESROMHeader()
    : mHeaderData(nullptr) {

}

SNESROMHeader::SNESROMHeader(const char *headerData)
    : mHeaderData(new HeaderData) {
    memcpy(static_cast<void *>(mHeaderData.get()), headerData, sizeof(HeaderData));

    assert(mHeaderData->m_ROMLayout & 0x20); //bit 0x20 is always set in the ROM layout
    assert(mHeaderData->m_countryCode < 0x0E);//country codes ranging from 0x0E to 0xFF are invalid
}

SNESROMHeader::SNESROMHeader(const SNESROMHeader &other)
    : mHeaderData(new HeaderData) {
    memcpy(static_cast<void *>(mHeaderData.get()), static_cast<void *>(other.mHeaderData.get()), sizeof(*mHeaderData.get()));
}

SNESROMHeader::SNESROMHeader(SNESROMHeader &&other)
    : mHeaderData(other.mHeaderData.release()) {
}

SNESROMHeader::~SNESROMHeader() {
}

const SNESROMHeader &SNESROMHeader::operator=(const SNESROMHeader &other) {
    SNESROMHeader header(other);

    swap(*this, header);
    return *this;
}

const SNESROMHeader &SNESROMHeader::operator=(SNESROMHeader && other) {
    swap(*this, other);
    return *this;
}

void swap(SNESROMHeader &lhs, SNESROMHeader &rhs) {
    using namespace std;
    swap(lhs.mHeaderData, rhs.mHeaderData);
}

std::string SNESROMHeader::getROMName() const {
    return std::string(mHeaderData->m_ROMName, mHeaderData->m_ROMName + 20);
}

bool SNESROMHeader::isLoROM() const {
    return (mHeaderData->m_ROMLayout & 0x01) != 0x01;
}

bool SNESROMHeader::isFastROM() const {
    return (mHeaderData->m_ROMLayout & 0x10);
}

SNESROMHeader::size_type SNESROMHeader::getROMSize() const {
    SNESROMHeader::size_type size = 1024;

    for(uint8_t i = 0; i < mHeaderData->m_ROMSize; ++i) {
        size *= 2;
    }

    return size;
}

int SNESROMHeader::getRAMSize() const {
    if(mHeaderData->m_ROMSize == 0) {
        return 0;
    } else {
        size_type ROMSize = 2048;
        for(uint8_t i = 1; i < mHeaderData->m_ROMSize; ++i) {
            ROMSize *= 2;
        }
        return ROMSize;
    }
}

int SNESROMHeader::getColorTransmissionSystem() const {
    if(mHeaderData->m_countryCode <= 0x01) {
        return NTSC;
    } else if(mHeaderData->m_countryCode <= 0x0C) {
        return PAL;
    } else {
        return NTSC;
    }
}

ROMAddress SNESROMHeader::getInterruptDest(NativeIV vector) const {
    return ROMAddress(isLoROM(), 0x00, mHeaderData->m_nativeInterruptVectors[static_cast<unsigned int>(vector)]);
}

ROMAddress SNESROMHeader::getInterruptDest(EmulationIV vector) const {
    return ROMAddress(isLoROM(), 0x00, mHeaderData->m_emulationInterruptVectors[static_cast<unsigned int>(vector)]);
}
