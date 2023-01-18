cmake_minimum_required(VERSION 3.18)

set(ENABLE_TEST 0)

project(LightWeightCommon)

add_library(LightWeightCommon STATIC)

target_sources(LightWeightCommon
PRIVATE
    io/filesystem_archive.cpp
    id/versioned_uid.cpp
    memory/memory.cpp
    memory/flight_ring.cpp
    string/name.cpp
    memory/tlsf/comm_tlsf.cpp
    log/client_log.cpp
)

target_include_directories(LightWeightCommon
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(LightWeightCommon
PUBLIC
    _CRT_SECURE_NO_WARNINGS
)

if(ENABLE_TEST)

    add_executable(flight_ring_test)
    target_sources(flight_ring_test
    PRIVATE
        test/flight_ring_test.cpp
    )

    target_link_libraries(flight_ring_test
    PRIVATE
        LightWeightCommon
    )

endif()