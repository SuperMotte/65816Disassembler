#ifndef FLAGREGISTER_HPP
#define FLAGREGISTER_HPP

#define NUMBER_OF_REGISTERFLAGS 8

/**
 * @brief The flagRegisterNames enum contains the names of the processor flags.
 */
enum flagRegisterNames : unsigned char {
    CARRY           = 0x01, //
    ZERO            = 0x02, //
    IRQ             = 0x04, //
    DECIMAL         = 0x08, //if set numbers are represented in BCD-code
    INDEX_SELECT    = 0x10, //0 => 16-bit XY, 1 => 8-bit XY
    MEMORY_SELECT   = 0x20, //0 => 16-bit A, 1 => 8-bit A
    SIGNED_OVERFLOW = 0x40, //
    NEGATIVE        = 0x80  //
};

/**
 * @brief The flagRegister class contains the states of the processorflags of a 65816 procssor.
 */
class flagRegister {
  private:
    /**
     * @brief m_flags is a char representing a bitmask representing the processorflags. The names of the bits (flags) are found in the flagRegisterNames enum.
     */
    unsigned char m_flags;
  public:
    /**
     * @brief flagRegister creates an instance of flagRegister and sets all flags to zero.
     */
    flagRegister();
    /**
     * @brief areFlagsSet checks if all specified processorflags are set.
     * @param flagRegisterName is a bitmask of the flags which will be tested.
     * @return true if the processorflag is set and false otherwise.
     */
    bool areFlagsSet(unsigned char flagRegisterNamesBitmask);
    /**
     * @brief setFlagRegister sets some specific processorflag to true.
     * @param flagRegisterName is a bitmask with all the flags which will be set.
     */
    void setFlagRegister(unsigned char flagRegisterNamesBitmask);
    /**
     * @brief setFlagRegister sets some specific processorflag to false.
     * @param flagRegisterName is a bitmask with all the flags which will be reset.
     */
    void resetFlagRegister(unsigned char flagRegisterNamesBitmask);
};
#endif // FLAGREGISTER_HPP
