cmake_minimum_required(VERSION 3.15)

project(MCK_FX VERSION 0.0.1)

add_subdirectory(deps/JUCE)

add_subdirectory(deps/MckJuce/Content)
set_property(TARGET MckBinaryData PROPERTY POSITION_INDEPENDENT_CODE ON)

add_subdirectory(Source/Delay)
add_subdirectory(Source/Trem)