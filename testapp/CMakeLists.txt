project(testapp)

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})

set(testapp_src
    main.cpp)

add_executable(testapp ${testapp_src})
target_link_libraries(testapp libsnesdisasm)
