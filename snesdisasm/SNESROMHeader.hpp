#ifndef SNESROMHEADER_HPP
#define SNESROMHEADER_HPP

//romhack.wikia.com/wiki/SNES_header
#include "ROMAddress.hpp"

#include <cstdint> // fixed sized integers
#include <string>
#include <memory>


/**
 * \brief The colorTransmissionSystem enum has the two possibilities: NTSC and PAL, both can be found in SNES ROMs but not together.
 */
enum colorTransmissionSystem {
    NTSC,
    PAL
};

/**
 * \brief This class ontains names for possible interrupt vectors for the native mode.
 */
class NativeIV {
  private:
    unsigned int idx;
    NativeIV(unsigned int x) : idx {x} {}
  public:
    explicit operator unsigned int() const {
        return idx;
    }
    constexpr static unsigned int count = 6;
    static NativeIV COP() {
        return NativeIV(0);
    }
    static NativeIV BRK() {
        return NativeIV(1);
    }
    static NativeIV ABORT() {
        return NativeIV(2);
    }
    static NativeIV NMT() {
        return NativeIV(3);
    }
    static NativeIV UNUSED() {
        return NativeIV(4);
    }
    static NativeIV IRQ() {
        return NativeIV(5);
    }
};

/**
 * \brief This class contains names for possible interrupt vectors for the emulation mode.
 */
class EmulationIV {
  private:
    unsigned int idx;
    EmulationIV(unsigned int x) : idx {x} {}
  public:
    explicit operator unsigned int() const {
        return idx;
    }
    constexpr static unsigned int count = 6;
    static EmulationIV COP() {
        return EmulationIV(0);
    }
    static EmulationIV UNUSED() {
        return EmulationIV(1);
    }
    static EmulationIV ABORT() {
        return EmulationIV(2);
    }
    static EmulationIV NMI() {
        return EmulationIV(3);
    }
    static EmulationIV RESET() {
        return EmulationIV(4);
    }
    static EmulationIV IRQ() {
        return EmulationIV(5);
    }
    static EmulationIV BRK() {
        return EmulationIV(5);    //compares positive to IRQ
    }
};

class SNESROMHeader {
  public:
    typedef unsigned int size_type;
    typedef uint16_t Address;
  private:
    static constexpr uint8_t m_ROMNameIndex = 0;
    static constexpr uint8_t m_ROMNameLength = 21;
    static constexpr uint8_t m_ROMLayoutIndex = 21;
    static constexpr uint8_t m_CartridgeTypeIndex = 22;
    static constexpr uint8_t m_ROMSizeIndex = 23;
    static constexpr uint8_t m_RAMSizeIndex = 24;
    static constexpr uint8_t m_CountryCodeIndex = 25;
    static constexpr uint8_t m_LocenseCodeIndex = 26;
    static constexpr uint8_t m_VersionNumber = 27;
    static constexpr uint8_t m_CheckSumComplementIndex = 28;
    static constexpr uint8_t m_SNESCheckSumIndex = 30;
    static constexpr uint8_t m_NativeInterruptVectorIndex = 36;
    static constexpr uint8_t m_EmulationInterruptVectorIndex = 52;

    /*! \brief A pointer to the header start. It is non-owning.
     *
     * \todo maybe we should convert it to a shared_ptr or weak_ptr
     */
    const uint8_t *m_HeaderData;

    //prevent copies
    SNESROMHeader(const SNESROMHeader &other) = delete;
    const SNESROMHeader &operator=(const SNESROMHeader &other) = delete;
  public:
    /*!
     * \brief Creates an invalid header
     */
    explicit SNESROMHeader();

    /*!
     * \brief SNESROMHeader creates an instance of SNESROMHeader with the data pointed at headerData.
     * \param headerData is a pointer to the beginning of the memory containing the SNES header.
     */
    explicit SNESROMHeader(const uint8_t *headerData);

    /*!
     * \brief tries to find the header
     * \param headerData the position to look for the header
     * \return true if it is found there
     */
    static bool mayBeThere(const uint8_t *headerData);

    /*!
     * \brief Returns true if m_HeaderData is set
     */
    operator bool() const { return m_HeaderData != nullptr; }

    /*!
     * \brief Move constructs from other. other becomes invalid
     */
    SNESROMHeader(SNESROMHeader &&other) noexcept;

    /*!
     * \brief Deletes the header
     */
    ~SNESROMHeader();

    /*!
     * \brief Move assigns the header.
     */
    const SNESROMHeader &operator=(SNESROMHeader && other) noexcept;

    /**
     * \brief getROMName creates a copy of the ROMName.
     * \return a copy of the ROMName.
     */
    std::string getROMName() const;

    /**
     * \brief isLoROM checks the header information about the ROM being a Lo- or a HiROM.
     * \return true if the ROM is according to it's SNES header is LoROM and false if the ROM is HiROM according to the SNES header information.
     */
    RomLayout layout() const;

    /**
     * \brief isFastROM checks the header information about the ROM being a fast ROM.
     * \return true if the ROM is according to it's SNES header a fast ROM.
     */
    bool isFastROM() const;
    /**
     * \brief getROMSize returns the size of the ROM according to the SNES header in bytes.
     * \return the size of the ROM in bytes.
     */
    size_type getROMSize() const;
    /**
     * \brief getRAMSize returns the size of the RAM according to the SNES header in bytes.
     * \return the size of the RAM in bytes.
     */
    int getRAMSize() const;
    /**
     * \brief getColorTransmissionSystem checks the color transmission system by reading the country code.
     * \return NTSC or PAL depending on the country code. This are symbolic integers found in the colorTransmissionSystem enum.
     */
    int getColorTransmissionSystem() const;

    /**
     * \brief returns the address of the given interupt entry
     */

    ROMAddress* getInterruptDest(NativeIV vector) const;
    ROMAddress* getInterruptDest(EmulationIV vector) const;
};

#endif // SNESROMHEADER_HPP
