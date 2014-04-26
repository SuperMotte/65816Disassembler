#include "CPUState.hpp"

CPUState::CPUState() :
    m_Accumulator(0),
    m_DataBank(0),
    m_IndexX(0),
    m_IndexY(0),
    m_StackPointer(nullptr),
    m_DirectPage(0),
    m_ProgramBank(0),
    m_ProgramCounter(0),
    m_FlagRegister(0x00)
{

}



uint16_t CPUState::getAccumulator() {
    if (areFlagsSet(MEMORY_SELECT)) {
        //if the flag is set, the accumulator uses the lower 8 bit
        return (m_Accumulator & 0x00FF);
    } else {
        //if the flag is not set, the accumulator uses 16 bit
        return m_Accumulator;
    }
}

uint16_t CPUState::getIndexX() {
    if (areFlagsSet(INDEX_SELECT)) {
        //if the flag is set, the indexX uses the lower 8 bit
        return (m_IndexX & 0x00FF);
    } else {
        //if the flag is not set, the indexX uses 16 bit
        return m_IndexX;
    }
}

uint16_t CPUState::getIndexY() {
    if (areFlagsSet(MEMORY_SELECT)) {
        //if the flag is set, the indexY uses the lower 8 bit
        return (m_IndexY & 0x00FF);
    } else {
        //if the flag is not set, the indexY uses 16 bit
        return m_IndexY;
    }
}

void CPUState::setAccumulator(uint16_t newValue, bool ignoreRegisterSize) {
    if (ignoreRegisterSize) {
        m_Accumulator = newValue;
    } else {
        if (areFlagsSet(MEMORY_SELECT)) {
            //set only the lower byte of m_Accumulator
            m_Accumulator = (m_Accumulator & 0xFF00) + (newValue & 0x00FF);
        } else {
            m_Accumulator = newValue;
        }
    }
}

void CPUState::setIndexX(uint16_t newValue, bool ignoreRegisterSize) {
    if (ignoreRegisterSize) {
        m_IndexX = newValue;
    } else {
        if (areFlagsSet(INDEX_SELECT)) {
            //set only the lower byte of m_Accumulator
            m_IndexX = (m_IndexX & 0xFF00) + (newValue & 0x00FF);
        } else {
            m_IndexX = newValue;
        }
    }
}

void CPUState::setIndexY(uint16_t newValue, bool ignoreRegisterSize) {
    if (ignoreRegisterSize) {
        m_IndexY = newValue;
    } else {
        if (areFlagsSet(INDEX_SELECT)) {
            //set only the lower byte of m_Accumulator
            m_IndexY = (m_IndexY & 0xFF00) + (newValue & 0x00FF);
        } else {
            m_IndexY = newValue;
        }
    }
}



bool CPUState::areFlagsSet(uint8_t flagRegisterNamesBitmask) const {
    return (m_FlagRegister & flagRegisterNamesBitmask) == flagRegisterNamesBitmask;
}

void CPUState::setFlagRegister(uint8_t flagRegisterNamesBitmask) {
    m_FlagRegister |= flagRegisterNamesBitmask;
}

void CPUState::resetFlagRegister(uint8_t flagRegisterNamesBitmask) {
    m_FlagRegister &= !flagRegisterNamesBitmask;
}
