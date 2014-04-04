/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2014  Tobias Loose <email>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "Instructions.hpp"
#include "Logger.hpp"

#include <sstream>

//http://wiki.superfamicom.org/snes/show/Jay's+ASM+Tutorial
enum AddressingMode : unsigned char {
    //der wert des parameters sei v
    //konkatenierung von daten sei :
    //      a : b, wenn a und b bytes sind ist demnach: (a << 8) | b
    //      a +: b, bedeutet, dass b an a konkateniert wird, aber a (temp) um 1 erhöht, wenn b überläuft. demnach: (a << 8) + b
    //      [s] sei der wert der, in der zelle mit adresse s steht
    //      flag(x) ist das flag x
    //      das ergebnis jeder gleichung ist eine adresse. eine ausnahme bildet IMMEDIATE, welches ein wert ist
    //      RX, RY, SR = RegisterX, RegisterY, StackRegister
    //      RX(H), RX(L) = Register X Hight, Register X Low
    //      
    IMMEDIATE,               //               v        v ist 1 oder 2 byte, je nach operation
                             //                        operationen mit 2 byte haben einen anderen opCode als solche mit
                             //                        einem byte. vielleicht sollte man IMMEDIATE deshalb auftrennen
    ABSOLUTE,                // DBR :     v            v ist 2 byte. DRB ist das data bank register 
    DIRECT,  //Zero page     //  00 : (DPR + v)        v ist 1 byte. DPR(H) ist 00 im emulationsmodus
                             //                        soll wirklich byte 0000:v adressiert werden, muss !v stehen
    ABSOLUTE_INDEXED_WITH_X, //  RX + (DBR : v)        v ist 2 byte, RX ist je nach flag(x) 1 oder 2 byte
    ABSOLUTE_INDEXED_WITH_Y, //  RY + (DBR : v)        ist v < 100, muss es !v sein
    ABSOLUTE_LONG,           //           v            v ist volle 3 byte
    DIRECT_INDEXED_WITH_X,   //  00 : (DPR + v + RX)   immer in bank 0. im 6502 emulation mode,
    DIRECT_INDEXED_WITH_Y,   //  00 : (DPR + v + RY)   ist es auch immer in page DPR. sonst springt es auf die nächste
    ACCUMULATOR,             //       A                es wird direkt im akkumulator gearbeitet
    IMPLIED,                 //                        vom opCode bestimmt
    STACK,                   //       SR               die adresse liegt in SR
    DIRECT_INDIRECT,         // DBR : s=[00 : DPR + v] v ist 1 byte. s sind 2 byte: (low : high)
    
    
    
    
    //IMMEDIATE_MEMORY_FLAG,           //
    //IMMEDIATE_INDEX_FLAG,            //
    //IMMEDIATE_8_BIT,                 //
    RELATIVE,                        //
    RELATIVE_LONG,                   //
    DIRECT_INDEXED_INDIRECT,         //registerDBR : (DataAt(value+registerD+registerX))
    DIRECT_INDIRECT_INDEXED,         //registerDBR : DataAt(??(maybe RAM?) : registerD+value) + registerY
    DIRECT_INDIRECT_LONG,            //long3Bytes(DataAt((value+registerD))
    DIRECT_INDIRECT_INDEXED_LONG,    //DataAt(long3Bytes(registerD + value)) + registerY
    ABSOLUTE_INDEXED_LONG,           //long3Bytes(value+registerX)
    STACK_RELATIVE,                  //value + stackValue
    STACK_RELATIVE_INDIRECT_INDEXED, //registerDBR : (value+stackValue+registerY)
    ABSOLUTE_INDIRECT,               //??(maybe RAM?) : DataAt(??(maybe DBR?) : value)
    ABSOLUTE_INDIRECT_LONG,          //
    ABSOLUTE_INDEXED_INDIRECT,       //     ??     : DataAt(value + registerX)
    IMPLIED_ACCUMULATOR,             //
    BLOCK_MOVE,                      //move #registerA bytes from value1:registerY to value2:registerX


    
    ABSOLUTE_INDEXED_LONG_WITH_X,
    PROGRAMMCOUNTER_RELATIVE,
    PROGRAMMCOUNTER_RELATIVE_LONG,
    STACK_INTERRUPT,
    RESERVED,
    DIRECT_INDIRECT_INDEXED_WITH_Y,
    DIRECT_INDIRECT_LONG_INDEXED_WITH_Y
};

/*
 * Direct Indexed Indirect (d,x)
 * Direct Indirect Indexed (d),y
 * Direct Indirect (d)
 * Direct Indirect Long [d]
 * Direct Indirect Indexed Long [d],y
 * Absolute
 * Absolute,x
 * Absolute,y
 * Absolute long
 * Absolute long indexed
 * Stack Relative Indirect Indexed (d,s),y
*/
//dir, dir, (dir) (dir, (dir) [dir] abs abs, abs, absl absl d,s (d,s)
//  x    y          x)   ,y               x    y        ,x        ,y

const char opCodes[256][4] = {
/* +         0x00   0x01   0x02   0x03   0x04   0x05   0x06   0x07   0x08   0x09   0x0A   0x0B   0x0C   0x0D   0x0E   0x0F*/
/*0x00*/    "BRK", "ORA", "COP", "ORA", "TSB", "ORA", "ASL", "ORA", "PHP", "ORA", "ASL", "PHD", "TSB", "ORA", "ASL", "ORA",
/*0x10*/    "BPL", "ORA", "ORA", "ORA", "TRB", "ORA", "ASL", "ORA", "CLC", "ORA", "INC", "TCS", "TRB", "ORA", "ASL", "ORA",
/*0x20*/    "JSR", "AND", "JSR", "AND", "BIT", "AND", "ROL", "AND", "PLP", "AND", "ROL", "PLD", "BIT", "AND", "ROL", "AND",
/*0x30*/    "BMI", "AND", "AND", "AND", "BIT", "AND", "ROL", "AND", "SEC", "AND", "DEC", "TSC", "BIT", "AND", "ROL", "AND",
/*0x40*/    "RTI", "EOR", "WDM", "EOR", "MVN", "EOR", "LSR", "EOR", "PHA", "EOR", "LSR", "PHK", "JMP", "EOR", "LSR", "EOR",
/*0x50*/    "BVC", "EOR", "EOR", "EOR", "MVN", "EOR", "LSR", "EOR", "CLI", "EOR", "PHY", "TCD", "JMP", "EOR", "LSR", "EOR",
/*0x60*/    "RTS", "ADC", "PER", "ADC", "STZ", "ADC", "ROR", "ADC", "PLA", "ADC", "ROR", "RTL", "JMP", "ADC", "ROR", "ADC",
/*0x70*/    "BVS", "ADC", "ADC", "ADC", "STZ", "ADC", "ROR", "ADC", "SEI", "ADC", "PLY", "TDC", "JMP", "ADC", "ROR", "ADC",
/*0x80*/    "BRA", "STA", "BRL", "STA", "STY", "STA", "STX", "STA", "DEY", "STA", "TXA", "PHB", "STY", "STA", "STX", "STA",
/*0x90*/    "BCC", "STA", "STA", "STA", "STY", "STA", "STX", "STA", "TYA", "STA", "TXS", "TXY", "STZ", "STA", "STZ", "STA",
/*0xA0*/    "LDY", "LDA", "LDX", "LDA", "LDY", "LDA", "LDX", "LDA", "TAY", "LDA", "TAX", "PLB", "LDY", "LDA", "LDX", "LDA",
/*0xB0*/    "BCS", "LDA", "LDA", "LDA", "LDY", "LDA", "LDX", "LDA", "CLV", "LDA", "TSX", "TYX", "LDY", "LDA", "LDX", "LDA",
/*0xC0*/    "CPY", "CMP", "REP", "CMP", "CPY", "CMP", "DEC", "CMP", "INY", "CMP", "DEX", "WAI", "CPY", "CMP", "DEC", "CMP",
/*0xD0*/    "BNE", "CMP", "CMP", "CMP", "PEI", "CMP", "DEC", "CMP", "CLD", "CMP", "PHX", "STP", "JMP", "CMP", "DEC", "CMP",
/*0xE0*/    "CPX", "SBC", "SEP", "SBC", "CPX", "SBC", "INC", "SBC", "INX", "SBC", "NOP", "XBA", "CPX", "SBC", "INC", "SBC",
/*0xF0*/    "BEQ", "SBC", "SBC", "SBC", "PEA", "SBC", "INC", "SBC", "SED", "SBC", "PLX", "XCE", "JSR", "SBC", "INC", "SBC"
};

uint8_t opCodeByteSize[256] = {
/* +         0x00   0x01   0x02   0x03   0x04   0x05   0x06   0x07   0x08   0x09   0x0A   0x0B   0x0C   0x0D   0x0E   0x0F*/
/*0x00*/      2,     2,     2,     2,     2,     2,     2,     2,     1,     2,     1,     1,     3,     3,     3,     4,
/*0x10*/      2,     2,     2,     2,     2,     2,     2,     2,     1,     3,     1,     1,     3,     3,     3,     4,
/*0x20*/      3,     2,     4,     2,     2,     2,     2,     2,     1,     2,     1,     1,     3,     3,     3,     4,
/*0x30*/      2,     2,     2,     2,     2,     2,     2,     2,     1,     3,     1,     1,     3,     3,     3,     4,
/*0x40*/      1,     2,     2,     2,     3,     2,     2,     2,     1,     2,     1,     1,     3,     3,     3,     4,
/*0x50*/      2,     2,     2,     2,     3,     2,     2,     2,     1,     3,     1,     1,     4,     3,     3,     4,
/*0x60*/      1,     2,     3,     2,     2,     2,     2,     2,     1,     2,     3,     1,     3,     3,     3,     4,
/*0x70*/      2,     2,     2,     2,     2,     2,     2,     2,     1,     3,     1,     1,     3,     3,     3,     4,
/*0x80*/      2,     2,     3,     2,     2,     2,     2,     2,     1,     2,     1,     1,     3,     3,     3,     4,
/*0x90*/      2,     2,     2,     2,     2,     2,     2,     2,     1,     3,     1,     1,     3,     3,     3,     4,
/*0xA0*/      2,     2,     2,     2,     2,     2,     2,     2,     1,     2,     1,     1,     3,     3,     3,     4,
/*0xB0*/      2,     2,     2,     2,     2,     2,     2,     2,     1,     3,     1,     1,     3,     3,     3,     4,
/*0xC0*/      2,     2,     2,     2,     2,     2,     2,     2,     1,     2,     1,     1,     3,     3,     3,     4,
/*0xD0*/      2,     2,     2,     2,     2,     2,     2,     2,     1,     3,     1,     1,     3,     3,     3,     4,
/*0xE0*/      2,     2,     2,     2,     2,     2,     2,     2,     1,     2,     1,     1,     3,     3,     3,     4,
/*0xF0*/      2,     2,     2,     2,     3,     2,     2,     2,     1,     3,     1,     1,     3,     3,     3,     4
};

AddressingMode opCodeAddressingMode[256] = {
/* +                0x00                    0x01                            0x02                        0x03                        0x04                    0x05                 0x06                           0x07                   0x08             0x09              0x0A       0x0B            0x0C                          0x0D                     0x0E                     0x0F                */
/*0x00*/ STACK                   , DIRECT_INDEXED_INDIRECT, STACK                        , STACK_RELATIVE                , DIRECT               , DIRECT               , DIRECT               , DIRECT_INDIRECT_LONG               , STACK  , IMMEDIATE              , ACCUMULATOR, STACK  , ABSOLUTE                 , ABSOLUTE               , ABSOLUTE               , ABSOLUTE_LONG,
/*0x10*/ PROGRAMMCOUNTER_RELATIVE, DIRECT_INDIRECT_INDEXED, DIRECT_INDIRECT              , DIRECT_INDIRECT_INDEXED_WITH_Y, DIRECT               , DIRECT_INDEXED_WITH_X, DIRECT_INDEXED_WITH_X, DIRECT_INDIRECT_LONG_INDEXED_WITH_Y, IMPLIED, ABSOLUTE_INDEXED_WITH_Y, ACCUMULATOR, IMPLIED, ABSOLUTE                 , ABSOLUTE_INDEXED_WITH_X, ABSOLUTE_INDEXED_WITH_X, ABSOLUTE_INDEXED_LONG_WITH_X,
/*0x20*/ ABSOLUTE                , DIRECT_INDEXED_INDIRECT, ABSOLUTE_LONG                , STACK_RELATIVE                , DIRECT               , DIRECT               , DIRECT               , DIRECT_INDIRECT_LONG               , STACK  , IMMEDIATE              , ACCUMULATOR, STACK  , ABSOLUTE                 , ABSOLUTE               , ABSOLUTE               , ABSOLUTE_LONG,
/*0x30*/ PROGRAMMCOUNTER_RELATIVE, DIRECT_INDIRECT_INDEXED, DIRECT_INDIRECT              , DIRECT_INDIRECT_INDEXED_WITH_Y, DIRECT_INDEXED_WITH_X, DIRECT_INDEXED_WITH_X, DIRECT_INDEXED_WITH_X, DIRECT_INDIRECT_LONG_INDEXED_WITH_Y, IMPLIED, ABSOLUTE_INDEXED_WITH_Y, ACCUMULATOR, IMPLIED, ABSOLUTE_INDEXED_WITH_X  , ABSOLUTE_INDEXED_WITH_X, ABSOLUTE_INDEXED_WITH_X, ABSOLUTE_INDEXED_LONG_WITH_X,
/*0x40*/ STACK                   , DIRECT_INDEXED_INDIRECT, RESERVED                     , STACK_RELATIVE                , BLOCK_MOVE           , DIRECT               , DIRECT               , DIRECT_INDIRECT_LONG               , STACK  , IMMEDIATE              , ACCUMULATOR, STACK  , ABSOLUTE                 , ABSOLUTE               , ABSOLUTE               , ABSOLUTE_LONG,
/*0x50*/ PROGRAMMCOUNTER_RELATIVE, DIRECT_INDIRECT_INDEXED, DIRECT_INDIRECT              , DIRECT_INDIRECT_INDEXED_WITH_Y, BLOCK_MOVE           , DIRECT_INDEXED_WITH_X, DIRECT_INDEXED_WITH_X, DIRECT_INDIRECT_LONG_INDEXED_WITH_Y, IMPLIED, ABSOLUTE_INDEXED_WITH_Y, STACK      , IMPLIED, ABSOLUTE_LONG            , ABSOLUTE_INDEXED_WITH_X, ABSOLUTE_INDEXED_WITH_X, ABSOLUTE_INDEXED_LONG_WITH_X,
/*0x60*/ STACK                   , DIRECT_INDEXED_INDIRECT, STACK                        , STACK_RELATIVE                , DIRECT               , DIRECT               , DIRECT               , DIRECT_INDIRECT_LONG               , STACK  , IMMEDIATE              , ACCUMULATOR, STACK  , ABSOLUTE_INDIRECT        , ABSOLUTE               , ABSOLUTE               , ABSOLUTE_LONG,
/*0x70*/ PROGRAMMCOUNTER_RELATIVE, DIRECT_INDIRECT_INDEXED, DIRECT_INDIRECT              , DIRECT_INDIRECT_INDEXED_WITH_Y, DIRECT_INDEXED_WITH_X, DIRECT_INDEXED_WITH_X, DIRECT_INDEXED_WITH_X, DIRECT_INDIRECT_LONG_INDEXED_WITH_Y, IMPLIED, ABSOLUTE_INDEXED_WITH_Y, STACK      , IMPLIED, ABSOLUTE_INDEXED_INDIRECT, ABSOLUTE_INDEXED_WITH_X, ABSOLUTE_INDEXED_WITH_X, ABSOLUTE_INDEXED_LONG_WITH_X,
/*0x80*/ PROGRAMMCOUNTER_RELATIVE, DIRECT_INDEXED_INDIRECT, PROGRAMMCOUNTER_RELATIVE_LONG, STACK_RELATIVE                , DIRECT               , DIRECT               , DIRECT               , DIRECT_INDIRECT_LONG               , IMPLIED, IMMEDIATE              , IMPLIED    , STACK  , ABSOLUTE                 , ABSOLUTE               , ABSOLUTE               , ABSOLUTE_LONG,
/*0x90*/ PROGRAMMCOUNTER_RELATIVE, DIRECT_INDIRECT_INDEXED, DIRECT_INDIRECT              , DIRECT_INDIRECT_INDEXED_WITH_Y, DIRECT_INDEXED_WITH_X, DIRECT_INDEXED_WITH_X, DIRECT_INDEXED_WITH_Y, DIRECT_INDIRECT_LONG_INDEXED_WITH_Y, IMPLIED, ABSOLUTE_INDEXED_WITH_Y, IMPLIED    , IMPLIED, ABSOLUTE                 , ABSOLUTE_INDEXED_WITH_X, ABSOLUTE_INDEXED_WITH_X, ABSOLUTE_INDEXED_LONG_WITH_X,
/*0xA0*/ IMMEDIATE               , DIRECT_INDEXED_INDIRECT, IMMEDIATE                    , STACK_RELATIVE                , DIRECT               , DIRECT               , DIRECT               , DIRECT_INDIRECT_LONG               , IMPLIED, IMMEDIATE              , IMPLIED    , STACK  , ABSOLUTE                 , ABSOLUTE               , ABSOLUTE               , ABSOLUTE_LONG,
/*0xB0*/ PROGRAMMCOUNTER_RELATIVE, DIRECT_INDIRECT_INDEXED, DIRECT_INDIRECT              , DIRECT_INDIRECT_INDEXED_WITH_Y, DIRECT_INDEXED_WITH_X, DIRECT_INDEXED_WITH_X, DIRECT_INDEXED_WITH_Y, DIRECT_INDIRECT_LONG_INDEXED_WITH_Y, IMPLIED, ABSOLUTE_INDEXED_WITH_Y, IMPLIED    , IMPLIED, ABSOLUTE_INDEXED_WITH_X  , ABSOLUTE_INDEXED_WITH_X, ABSOLUTE_INDEXED_WITH_Y, ABSOLUTE_INDEXED_LONG_WITH_X,
/*0xC0*/ IMMEDIATE               , DIRECT_INDEXED_INDIRECT, IMMEDIATE                    , STACK_RELATIVE                , DIRECT               , DIRECT               , DIRECT               , DIRECT_INDIRECT_LONG               , IMPLIED, IMMEDIATE              , IMPLIED    , IMPLIED, ABSOLUTE                 , ABSOLUTE               , ABSOLUTE               , ABSOLUTE_LONG,
/*0xD0*/ PROGRAMMCOUNTER_RELATIVE, DIRECT_INDIRECT_INDEXED, DIRECT_INDIRECT              , DIRECT_INDIRECT_INDEXED_WITH_Y, STACK                , STACK                , DIRECT_INDEXED_WITH_X, DIRECT_INDIRECT_LONG_INDEXED_WITH_Y, IMPLIED, ABSOLUTE_INDEXED_WITH_Y, STACK      , IMPLIED, ABSOLUTE_INDIRECT_LONG   , ABSOLUTE_INDEXED_WITH_X, ABSOLUTE_INDEXED_WITH_X, ABSOLUTE_INDEXED_LONG_WITH_X,
/*0xE0*/ IMMEDIATE               , DIRECT_INDEXED_INDIRECT, IMMEDIATE                    , STACK_RELATIVE                , DIRECT               , DIRECT               , DIRECT               , DIRECT_INDIRECT_LONG               , IMPLIED, IMMEDIATE              , IMPLIED    , IMPLIED, ABSOLUTE                 , ABSOLUTE               , ABSOLUTE               , ABSOLUTE_LONG,
/*0xF0*/ PROGRAMMCOUNTER_RELATIVE, DIRECT_INDIRECT_INDEXED, DIRECT_INDIRECT              , DIRECT_INDIRECT_INDEXED_WITH_Y, STACK                , DIRECT_INDEXED_WITH_X, DIRECT_INDEXED_WITH_X, DIRECT_INDIRECT_LONG_INDEXED_WITH_Y, IMPLIED, ABSOLUTE_INDEXED_WITH_Y, STACK      , IMPLIED, ABSOLUTE_INDEXED_INDIRECT, ABSOLUTE_INDEXED_WITH_X, ABSOLUTE_INDEXED_WITH_X, ABSOLUTE_INDEXED_LONG_WITH_X
};

Instruction::Instruction(uint8_t* data)
    : m_OpCode{data[0]}
{
    if(size() > 1) m_Argument.as8.at1 = data[1];
    if(size() > 2) m_Argument.as8.at2 = data[2];
    if(size() > 3) m_Argument.as8.at3 = data[3];
}

uint8_t Instruction::size() const {
    /*
     //influenced opCodes by the size of register M are 09, 29, 49, 69 (tihihi^^), 89, A9, C9, E9
            if(registerMIs16Byte) {
                switch(m_opCode) {
                case 0x09:
                case 0x29:
                case 0x49:
                case 0x69:
                case 0x89:
                case 0xA9:
                case 0xC9:
                case 0xE9:
                    ++size;
                }
            }
            //influenced opCodes by the size of register X are A0, A2, C0, E0
            if(registerXIs16Byte) {
                switch(m_opCode) {
                case 0xA0:
                case 0xA2:
                case 0xC0:
                case 0xE0:
                    ++size;
                }
            }
    */
    return opCodeByteSize[m_OpCode];
}

bool Instruction::isJump() const {
    switch(m_OpCode) {
    case 0x10: //Branch if Plus
    case 0x20: //Jump to Subroutine
    case 0x30: //Branch if Minus
    case 0x22: //Jump to Subroutine
    case 0x4C: //Jump
    case 0x5C: //Jump
    case 0x6C: //Jump
    case 0x7C: //Jump
    case 0x80: //Branch Always
    case 0xD0: //Branch if Not Equal
    case 0xDC: //Jump
    case 0xFC: //Jump to Subroutine
        return true;
    default:
        return false;
    }
}

std::string toHexStr(uint8_t s) {
    std::stringstream ss;
    ss << std::hex << int(s);
    return ss.str();
}

std::string toHexStr(const Instruction::Argument_t& arg, uint8_t bytes) {
    std::string s;
    
    if(bytes >= 1) s += toHexStr(arg.as8.at1);
    if(bytes >= 2) s += " " + toHexStr(arg.as8.at2);
    if(bytes >= 3) s += " " + toHexStr(arg.as8.at3);
    
    return s;
}

std::string Instruction::stringify() const {
    std::string s(opCodes[m_OpCode]);
    if(bytes() > 1) { //this instruction takes more than one byte. there is an argument
        AddressingMode mode = opCodeAddressingMode[m_OpCode];
        switch(mode) {
        
            default:
                s += " " + toHexStr(m_Argument, size()-1) + " (unknown mode)";
        }
    }
    
    return s;
}
