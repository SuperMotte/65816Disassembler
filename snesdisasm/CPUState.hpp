#ifndef REGISTERSTATE_HPP
#define REGISTERSTATE_HPP

#include <cstdint>

#define NUMBER_OF_REGISTERFLAGS 8

/**
 * \brief The flagRegisterNames enum contains the names of the processor flags.
 */
enum FlagRegisterNames : unsigned char {
    CARRY           = 0x01, //
    ZERO            = 0x02, //
    IRQ             = 0x04, //
    DECIMAL         = 0x08, //if set numbers are represented in BCD-code
    INDEX_SELECT    = 0x10, //0 => 16-bit XY, 1 => 8-bit XY
    MEMORY_SELECT   = 0x20, //0 => 16-bit A, 1 => 8-bit A
    SIGNED_OVERFLOW = 0x40, //
    NEGATIVE        = 0x80  //
};

class CPUState {
private:
    uint16_t m_Accumulator;
    uint16_t m_IndexX;
    uint16_t m_IndexY;
    uint16_t *m_StackPointer;
    uint8_t m_DataBank;
    uint16_t m_DirectPage;
    uint8_t m_ProgramBank;
    uint16_t m_ProgramCounter;
    /**
     * \brief m_FlagRegister is a bitmask representing the processorflags. The names of the bits (flags) are found in the flagRegisterNames enum.
     */
    uint8_t m_FlagRegister;
public:
    CPUState();


    /**
     * @brief getAccumulator
     * @return the lower bytevalue of the accumulator if the accumulatorsize is 8 bit or the full 16 bit if the size is 16 bit.
     */
    uint16_t getAccumulator();
    /**
     * @brief getIndexX
     * @return the lower bytevalue of the register X if the register X size is 8 bit or the full 16 bit if the size is 16 bit.
     */
    uint16_t getIndexX();
    /**
     * @brief getIndexY
     * @return the lower bytevalue of the register Y if the register Y size is 8 bit or the full 16 bit if the size is 16 bit.
     */
    uint16_t getIndexY();
    /**
     * @brief setAccumulator sets the accumulator to the given value.
     * @param newValue the new value the accumulator will have.
     * @param ignoreRegisterSize if true, the 16 bits of new value will be put in the accumulator otherwise it will put only as much bits in the accumulator as there is space in it.
     */
    void setAccumulator(uint16_t newValue, bool ignoreRegisterSize=false);
    /**
     * @brief setIndexX sets the register X to the given value.
     * @param newValue the new value the register X will have.
     * @param ignoreRegisterSize if true, the 16 bits of new value will be put in the register X otherwise it will put only as much bits in the register X as there is space in it.
     */
    void setIndexX(uint16_t newValue, bool ignoreRegisterSize=false);
    /**
     * @brief setIndexY sets the register Y to the given value.
     * @param newValue the new value the register Y will have.
     * @param ignoreRegisterSize if true, the 16 bits of new value will be put in the register Y otherwise it will put only as much bits in the register Y as there is space in it.
     */
    void setIndexY(uint16_t newValue, bool ignoreRegisterSize=false);



    //This was merged from the old FlagRegister class.
    /**
     * \brief areFlagsSet checks if all specified processorflags are set.
     * \param flagRegisterNameBitmask is a bitmask where all bits which are set to 1 will be tested.
     * \return true if all processorflags in flagRegisterNameBitmask are set and false otherwise.
     */
    bool areFlagsSet(uint8_t flagRegisterNamesBitmask) const;
    /**
     * \brief setFlagRegister sets some specific processorflag to true.
     * \param flagRegisterNameBitmask is a bitmask where all bits which are set to 1 will be set.
     */
    void setFlagRegister(uint8_t flagRegisterNamesBitmask);
    /**
     * \brief setFlagRegister sets some specific processorflag to false.
     * \param flagRegisterNameBitmask is a bitmask where all bits which are set to 1 will be reset.
     */
    void resetFlagRegister(uint8_t flagRegisterNamesBitmask);


    //automatically generated getter- and setter-functions by Qt Creator
    uint16_t *StackPointer() const;
    void setStackPointer(uint16_t *StackPointer);
    uint8_t DataBank() const;
    void setDataBank(const uint8_t &DataBank);
    uint16_t DirectPage() const;
    void setDirectPage(const uint16_t &DirectPage);
    uint8_t ProgramBank() const;
    void setProgramBank(const uint8_t &ProgramBank);
    uint16_t ProgramCounter() const;
    void setProgramCounter(const uint16_t &ProgramCounter);
};

#endif // REGISTERSTATE_HPP
