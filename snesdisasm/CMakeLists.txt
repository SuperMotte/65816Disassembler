project(snesdisasm)

set(snesdisasm_src 
    Instructions.cpp
    FlagRegister.cpp
    Logger.cpp
    SNESROM.cpp
    SNESROMHeader.cpp
    Disasm.cpp
    CPUState.cpp
)

set(snesdisasm_VERSION_MAJOR 0)
set(snesdisasm_VERSION_MINOR 1)

configure_file("snesdisasmConfig.in.hpp"
               "snesdisasmConfig.hpp")

include_directories("${snesdisasm_BINARY_DIR}")

add_library(libsnesdisasm ${snesdisasm_src})
