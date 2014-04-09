#include "globals.h"

bool systemIsLittleEndian = true;
void testEndian() {
    unsigned int EndianTestInt=255;
    unsigned char* EndianTestChar = (unsigned char*)&EndianTestInt;
    systemIsLittleEndian = (EndianTestChar[0]==0xFF);
}
