#ifndef SNESROMHEADER_HPP
#define SNESROMHEADER_HPP

#include <cstdint> // fixed sized integers
#include <string>
#include <memory>

/**
 * @brief The colorTransmissionSystem enum has the two possibilities: NTSC and PAL, both can be found in SNES ROMs but not together.
 */
enum colorTransmissionSystem {
    NTSC,
    PAL
};

/**
 * @brief This class ontains names for possible interrupt vectors for the native mode.
 */
class NativeIV {
  private:
    unsigned int idx;
    NativeIV(unsigned int x) : idx {x} {}
  public:
    explicit operator unsigned int() const {
        return idx;
    };
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
 * @brief This class contains names for possible interrupt vectors for the emulation mode.
 */
class EmulationIV {
  private:
    unsigned int idx;
    EmulationIV(unsigned int x) : idx {x} {}
  public:
    explicit operator unsigned int() const {
        return idx;
    };
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
    struct HeaderData { //this is a POD
        uint8_t m_ROMName[21];                 //21 chars representing the name of the ROM, filled with spaces for padding
        uint8_t m_ROMLayout;
        uint8_t m_cartridgeType;
        uint8_t m_ROMSize;                     //2^m_ROMSize kilobytes, the size of the ROM on the cartridge
        uint8_t m_RAMSize;                     //2^m_RAMSize kilobytes, the size of the RAM on the cartridge (EXCLUDING the RAM on the SNES system)
        uint8_t m_countryCode;
        uint8_t m_licenseCode;
        uint8_t m_versionNumber;
        uint16_t m_checkSumComplement;
        uint16_t m_SNESCheckSum;
        uint32_t m_unknownData;                 //the use of this is unknown
        Address m_nativeInterruptVectors[NativeIV::count];
        uint32_t m_unknownData2;                //the use of this is unknown
        Address m_emulationInterruptVectors[EmulationIV::count];
    };

    std::unique_ptr<HeaderData> mHeaderData;
  public:
    /**
     * @brief Creates an invalid header
     */
    explicit SNESROMHeader();

    /**
     * @brief SNESROMHeader creates an instance of SNESROMHeader with the data pointed at headerData.
     * @param headerData is a pointer to the beginning of the memory containing the SNES header.
     */
    explicit SNESROMHeader(const char *headerData);

    /**
     * @brief Move constructs from other. other becomes invalid
     */
    SNESROMHeader(SNESROMHeader &&other);

    /**
     * @brief Copy constructs from other. This will duplicate the internal state.
     */
    SNESROMHeader(const SNESROMHeader &other);

    /**
     * @brief Deletes the header
     */
    ~SNESROMHeader();

    const SNESROMHeader &operator=(const SNESROMHeader &other);

    const SNESROMHeader &operator=(SNESROMHeader && other);

    friend void swap(SNESROMHeader &lhs, SNESROMHeader &rhs);

    /**
     * @brief getROMName creates a copy of the ROMName.
     * @return a copy of the ROMName.
     */
    std::string getROMName() const;

    /**
     * @brief isLoROM checks the header information about the ROM being a Lo- or a HiROM.
     * @return true if the ROM is according to it's SNES header is LoROM and false if the ROM is HiROM according to the SNES header information.
     */
    bool isLoROM() const;
    /**
     * @brief isFastROM checks the header information about the ROM being a fast ROM.
     * @return true if the ROM is according to it's SNES header a fast ROM.
     */
    bool isFastROM() const;
    /**
     * @brief getROMSize returns the size of the ROM according to the SNES header in bytes.
     * @return the size of the ROM in bytes.
     */
    size_type getROMSize() const;
    /**
     * @brief getRAMSize returns the size of the RAM according to the SNES header in bytes.
     * @return the size of the RAM in bytes.
     */
    int getRAMSize() const;
    /**
     * @brief getColorTransmissionSystem checks the color transmission system by reading the country code.
     * @return NTSC or PAL depending on the country code. This are symbolic integers found in the colorTransmissionSystem enum.
     */
    int getColorTransmissionSystem() const;

    /**
     * @brief returns the address of the given interupt entry
     */
    Address getInterruptDest(NativeIV vector) const;
    Address getInterruptDest(EmulationIV vector) const;
};

#endif // SNESROMHEADER_HPP
