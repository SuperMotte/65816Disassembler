cmake_minimum_required(VERSION 2.8.12)

#enable c++x11 project wide
add_definitions("-std=c++0x")
#and hide ccache warnings
#add_definitions("-Qunused-arguments")

add_subdirectory(snesdisasm)
add_subdirectory(testapp)
