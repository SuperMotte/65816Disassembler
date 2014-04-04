#include "SNESROM.hpp"
#include "Logger.hpp"
#include <fstream>
#include <assert.h>

SNESROM::SNESROM(const std::string &ROMImagePath)
    : m_actualImageData(nullptr), m_headerlessImageData(nullptr) {
    std::ifstream imageDataStream(ROMImagePath, std::ifstream::in | std::ifstream::binary);

    if(!imageDataStream.is_open()) {
        LOG_SRC(ERROR, "Couldn't open ROMImageStream" + ROMImagePath);
        return;
    }

    //get the size of the ROM data
    imageDataStream.seekg(0, std::ios_base::end);
    const auto ROMDataSize = imageDataStream.tellg();
    imageDataStream.seekg(0, std::ios_base::beg);

    m_actualImageData = std::unique_ptr<char[]>(new char[ROMDataSize]);
    imageDataStream.read(m_actualImageData.get(), ROMDataSize);

    assert(ROMDataSize % 512 == 0);

    m_hasSMCHeader = (ROMDataSize % 1024 != 0);
    m_headerlessImageData = m_actualImageData.get() + ROMDataSize % 1024;

    if(m_hasSMCHeader) {
        LOG_SRC(STATE, "ROM has a SMC-Header");
        if(m_actualImageData[2] == 0x00) {
            m_isLoROM = true;
        } else {
            if(m_actualImageData[2] == 0x30) {
                m_isLoROM = false;
            } else {
                LOG_SRC(ERROR, "Unbekanntes ROM Layout im SMC-Header --- "
                        "http://romhack.wikia.com/wiki/SMC_header#Super_Magicom_.2A.smc");
            }
        }
    } else {
        LOG_SRC(STATE, "ROM has no SMC-Header");
    }

    if(m_isLoROM) {
        LOG_SRC(STATE, "LoROM");
    } else {
        LOG_SRC(STATE, "HiROM");
    }

    auto headerStartAddress = ROMAddressToImageAddress(0xFFC0);
    m_SNESROMHeader = SNESROMHeader(m_actualImageData.get() + headerStartAddress);

    LOG_SRC(STATE, "Detected ROM name: " + m_SNESROMHeader.getROMName());

    imageDataStream.close();
}

SNESROM::~SNESROM() {
}

void SNESROM::overwriteLoROMHiROM(bool isLoROM) {
    if(m_isLoROM != isLoROM) {
        m_isLoROM = isLoROM;
        LOG_SRC(WARNING, "Overwritten m_isLoROM");
    }
}

int SNESROM::imageAddressToROMAddress(int imageAddress) const {
    int ROMAddress;
    if(m_isLoROM) {
        int bankID = imageAddress / 0x8000;
        //map to the second mirror if needed
        if(bankID == 0x7E || bankID == 0x7F) {
            bankID += 0x80;
        }
        ROMAddress = bankID * 0x10000 + imageAddress % 0x8000 + 0x8000;
    } else {
        //it is a HiROM
        ROMAddress = 0xC00000 + imageAddress;
    }
    if(m_hasSMCHeader) {
        ROMAddress -= 512;
    }
    return ROMAddress;
}

int SNESROM::ROMAddressToImageAddress(int ROMAddress) const {
    int imageAddress;
    if(m_isLoROM) {
        int bankID = ROMAddress / 0x10000;
        imageAddress = bankID * 0x8000 + ROMAddress & 0xFFFF - 0x8000;
    } else {
        //it is a HiROM
        imageAddress = ROMAddress & 0x3FFFFF;
    }
    if(m_hasSMCHeader) {
        imageAddress += 512;
    }
    return imageAddress;
}

void SNESROM::copyBytes(uint8_t *destination, SNESROM::Address ROMAddress, size_t numberOfBytesToCopy) const {
    for(unsigned int i = 0; i < numberOfBytesToCopy; ++i) {
        destination[i] = m_actualImageData[ROMAddressToImageAddress(ROMAddress + i)];
    }
}

Instruction SNESROM::operator[](SNESROM::Address rom_address) const {
    uint8_t *ptr = (uint8_t *)(m_actualImageData.get());
    return Instruction::fromData(ptr + ROMAddressToImageAddress(rom_address));
}

const SNESROMHeader &SNESROM::header() const {
    return m_SNESROMHeader;
}
