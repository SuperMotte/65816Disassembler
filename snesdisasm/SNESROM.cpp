#include "SNESROM.hpp"
#include "Logger.hpp"
#include <fstream>
#include <assert.h>

std::vector<uint8_t> readBytesFromFile(const std::string &fileName)
{
    std::vector<uint8_t> vec;
    std::ifstream imageDataStream(fileName, std::ifstream::in | std::ifstream::binary);

    if(!imageDataStream.is_open()) {
        return vec;
    }

    imageDataStream.seekg(0, std::ios_base::end);
    const auto fileSize = imageDataStream.tellg();
    imageDataStream.seekg(0, std::ios_base::beg);

    vec.resize(fileSize);
    char *target = reinterpret_cast<char*>(&vec[0]);

    imageDataStream.read(target, fileSize);
    imageDataStream.close();

    vec.shrink_to_fit();

    return vec;
}

#include <iostream>

int imageAddressToROMAddress(ImageAddress imageAddress, bool m_isLoROM) {
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

    return ROMAddress -= 512;
}


SNESROM::SNESROM(const std::string &ROMImagePath)
    : m_actualImageData(readBytesFromFile(ROMImagePath)),
      m_headerlessImageData(nullptr){

    assert( m_actualImageData.size() % 512 == 0);
    assert( m_actualImageData.size() > 0);

    m_headerlessImageData = &m_actualImageData[0] +  m_actualImageData.size() % 1024;

    //this implies a SMC header
    if( m_actualImageData.size() % 1024 != 0){
        m_SMCHeader.load(&m_actualImageData[0]);
        LOG_SRC(STATE, "ROM has a SMC-Header");

        //since we have a SMC header, we ask it for lo/hi-rom status
        //not that this might be faulty so we gonna ignore that anyway
        RomLayout layout = m_SMCHeader.layout();

        //now that we know what rom-type it is, we can use that to find the header
        ImageAddress headerAddress = ROMAddress(0x00, 0xFFC0).toImageAddress(layout);

        //it is there. nice
        if(SNESROMHeader::isThere(m_headerlessImageData + headerAddress)){
           m_SNESROMHeader = SNESROMHeader(m_headerlessImageData + headerAddress);
        }else{
            LOG_SRC(WARNING, "SMC-Header lies about ROM layout");
        }
    }

    if(!m_SNESROMHeader){
        if(SNESROMHeader::isThere(m_headerlessImageData + 0x7fc0)){
            //headerless lorom
            m_SNESROMHeader = SNESROMHeader(m_headerlessImageData + 0x7fc0);
        }else if(SNESROMHeader::isThere(m_headerlessImageData + 0xffc0)){
            //headerless hirom
            m_SNESROMHeader = SNESROMHeader(m_headerlessImageData + 0xffc0);
        }else{
            LOG_SRC(ERROR, "There is no SNES header");
        }
    }
}

SNESROM::SNESROM(SNESROM &&other)
    : m_actualImageData(std::move(other.m_actualImageData)),
      m_headerlessImageData(other.m_headerlessImageData),
      m_SNESROMHeader(std::move(other.m_SNESROMHeader)),
      m_SMCHeader(std::move(other.m_SMCHeader)){
    other.m_headerlessImageData = nullptr;
}

SNESROM::~SNESROM() {
}

void SNESROM::copyBytes(uint8_t *destination, ROMAddress rom_address, size_t numberOfBytesToCopy) const {
    for(unsigned int i = 0; i < numberOfBytesToCopy; ++i) {
        destination[i] = m_headerlessImageData[rom_address.toImageAddress(m_SNESROMHeader.layout())];
    }
}

const uint8_t *SNESROM::operator[](ROMAddress rom_address) const {
    return &m_headerlessImageData[rom_address.toImageAddress(m_SNESROMHeader.layout())];
}

const SNESROMHeader &SNESROM::header() const {
    return m_SNESROMHeader;
}
