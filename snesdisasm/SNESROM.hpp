#ifndef SNESROM_HPP
#define SNESROM_HPP

#include "FlagRegister.hpp"
#include "SNESROMHeader.hpp"
#include "Instructions.hpp"
#include "SMCHeader.hpp"

#include "ROMAddress.hpp"

#include <memory>
#include <vector>

class SNESROM {
  private:
    std::vector<uint8_t> m_actualImageData; //this is the complete data of the ROM-Image
    uint8_t *m_headerlessImageData;    //this is a pointer which points to the beginning of the ROM-Image data ignoring the SMC-header,
    //  this is the same as m_actualROMData when no SMC-header or with an offset +512 if containing a SMC-header
    bool  m_isLoROM;                //true if the image is from a LoROM or false otherwise
    SNESROMHeader m_SNESROMHeader;  //the header of the SNES ROM
    SMCHeader m_SMCHeader;

    //prevent copying a rom
    SNESROM(const SNESROM &other) = delete;
    SNESROM &operator=(const SNESROM &other) = delete;
  public:
    typedef SNESROMHeader::Address Address;

    SNESROM(const std::string &ROMImagePath);
    SNESROM(SNESROM &&other);
    ~SNESROM();

    void overwriteLoROMHiROM(bool isLoROM);

    int imageAddressToROMAddress(ImageAddress imageAddress) const;
    ImageAddress ROMAddressToImageAddress(int ROMAddress) const;

    void copyBytes(uint8_t *destination, Address ROMAddress, size_t numberOfBytesToCopy) const;

    /**
     * \brief Returns a ptr to the byte at a given address
     */
<<<<<<< HEAD
    uint8_t *operator[](Address romAddress) const;
=======
    const uint8_t *operator[](Address rom_address) const;
>>>>>>> origin/master
    const SNESROMHeader &header() const;
};

#endif // SNESROM_HPP
