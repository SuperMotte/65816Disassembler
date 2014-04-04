#include "flagRegister.hpp"
#include <assert.h>



FlagRegister::FlagRegister() {
    //set all flags to zero
    m_flags = 0x00;
}

bool FlagRegister::areFlagsSet(unsigned char flagRegisterNamesBitmask) const {
    return (m_flags & flagRegisterNamesBitmask) == flagRegisterNamesBitmask;
}

void FlagRegister::setFlagRegister(unsigned char flagRegisterNamesBitmask) {
    m_flags |= flagRegisterNamesBitmask;
}

void FlagRegister::resetFlagRegister(unsigned char flagRegisterNamesBitmask) {
    m_flags &= !flagRegisterNamesBitmask;
}
