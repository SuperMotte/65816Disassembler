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

    return vec;
}

SNESROM::SNESROM(const std::string &ROMImagePath)
    : m_actualImageData(readBytesFromFile(ROMImagePath)),
      m_headerlessImageData(nullptr), m_isLoROM(false) {

    assert( m_actualImageData.size() % 512 == 0);

    m_headerlessImageData = &m_actualImageData[0] +  m_actualImageData.size() % 1024;

    //this implies a SMC header
    if( m_actualImageData.size() % 1024 != 0){
        m_SMCHeader.load(&m_actualImageData[0]);
        LOG_SRC(STATE, "ROM has a SMC-Header");

        //since we have a SMC header, we ask it for lo/hi-rom status
        //not that this might be faulty so we gonna ignore that anyway
        m_isLoROM = m_SMCHeader.isLoROM();

        //now that we know what rom-type it is, we can use that to find the header
        ImageAddress headerAddress = ROMAddressToImageAddress(0xFFC0);

        //it is there. nice
        if(SNESROMHeader::isThere(&m_actualImageData[headerAddress])){
           m_SNESROMHeader = SNESROMHeader(&m_actualImageData[headerAddress]);
        }else{
            LOG_SRC(WARNING, "SMC-Header lies about ROM layout");
        }
    }

    if(!m_SNESROMHeader){
        //not loaded. no clue where it could be. we probe for it
        if(m_SMCHeader){
            if(SNESROMHeader::isThere(&m_actualImageData[0x81c0])){
                //headerless lorom
                m_isLoROM = true;
            }else if(SNESROMHeader::isThere(&m_actualImageData[0x101c0])){
                //headerless hirom
                m_isLoROM = false;
            }else{
                LOG_SRC(ERROR, "There is no SNES header");
            }
        }else{
            if(SNESROMHeader::isThere(&m_actualImageData[0x7fc0])){
                //headered lorom
                m_isLoROM = true;
            }else if(SNESROMHeader::isThere(&m_actualImageData[0xffc0])){
                //headered hirom
                m_isLoROM = false;
            }else{
                LOG_SRC(ERROR, "There is no SNES header");
            }
        }

        ImageAddress headerAddress = ROMAddressToImageAddress(0xFFC0);
        m_SNESROMHeader = SNESROMHeader(&m_actualImageData[headerAddress]);

        if(m_SNESROMHeader){
            LOG_SRC(STATE, "found SNES header by probing.");
        }else{
            LOG_SRC(ERROR, "could not find any SNES header!");
        }
    }
}

SNESROM::SNESROM(SNESROM &&other)
    : m_actualImageData(std::move(other.m_actualImageData)),
      m_headerlessImageData(other.m_headerlessImageData),
      m_isLoROM(other.m_isLoROM),
      m_SNESROMHeader(std::move(other.m_SNESROMHeader)),
      m_SMCHeader(std::move(other.m_SMCHeader)){
    other.m_headerlessImageData = nullptr;
}

SNESROM::~SNESROM() {
}

void SNESROM::overwriteLoROMHiROM(bool isLoROM) {
    if(m_isLoROM != isLoROM) {
        m_isLoROM = isLoROM;
        LOG_SRC(WARNING, "Overwritten m_isLoROM");
    }
}

int SNESROM::imageAddressToROMAddress(ImageAddress imageAddress) const {
    int ROMAddress;
    if(m_SMCHeader) {
        ROMAddress -= 512;
    }

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

    if(m_SMCHeader) {
        ROMAddress -= 512;
    }
    return ROMAddress;
}

ImageAddress SNESROM::ROMAddressToImageAddress(int ROMAddress) const {
    ImageAddress imageAddress;
    if(m_isLoROM) {
        int bankID = ROMAddress / 0x10000;
        imageAddress = bankID * 0x8000 + ROMAddress & 0xFFFF - 0x8000;
    } else {
        //it is a HiROM
        imageAddress = ROMAddress & 0x3FFFFF;
    }
    if(m_SMCHeader) {
        imageAddress += 512;
    }
    return imageAddress;
}

void SNESROM::copyBytes(uint8_t *destination, SNESROM::Address ROMAddress, size_t numberOfBytesToCopy) const {
    for(unsigned int i = 0; i < numberOfBytesToCopy; ++i) {
        destination[i] = m_actualImageData[ROMAddressToImageAddress(ROMAddress + i)];
    }
}

const uint8_t *SNESROM::operator[](SNESROM::Address rom_address) const {
    return &m_actualImageData[ROMAddressToImageAddress(rom_address)];
}

const SNESROMHeader &SNESROM::header() const {
    return m_SNESROMHeader;
}
