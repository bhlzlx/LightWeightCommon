cmake_minimum_required(VERSION 3.18)

project(LightWeightCommon)


add_library(LightWeightCommon STATIC)

target_sources(LightWeightCommon
PRIVATE
    io/filesystem_archive.cpp
)

target_include_directories(LightWeightCommon
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)