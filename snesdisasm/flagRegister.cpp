#include "flagRegister.hpp"
#include <assert.h>



flagRegister::flagRegister() {
    //set all flags to zero
    m_flags = 0x00;
}

bool flagRegister::areFlagsSet(unsigned char flagRegisterNamesBitmask) {
    return (m_flags & flagRegisterNamesBitmask) == flagRegisterNamesBitmask;
}

void flagRegister::setFlagRegister(unsigned char flagRegisterNamesBitmask) {
    m_flags |= flagRegisterNamesBitmask;
}

void flagRegister::resetFlagRegister(unsigned char flagRegisterNamesBitmask) {
    m_flags &= !flagRegisterNamesBitmask;
}
