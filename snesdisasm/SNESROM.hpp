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

    void copyBytes(uint8_t *destination, ROMAddress rom_address, size_t numberOfBytesToCopy) const;
    RomLayout checkRomLayout();
    /**
     * \brief Returns a ptr to the byte at a given address
     */
    const uint8_t *operator[](ROMAddress rom_address) const;

    const SNESROMHeader &header() const;
};

#endif // SNESROM_HPP
