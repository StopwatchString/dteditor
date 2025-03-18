cmake_minimum_required(VERSION 3.30)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/instrset.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vector_convert.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vectorclass.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vectorf128.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vectorf256.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vectorf256e.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vectorf512.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vectorf512e.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vectorfp16.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vectorfp16e.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vectori128.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vectori256.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vectori256e.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vectori512.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vectori512e.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vectori512s.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vectori512se.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vectormath_common.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vectormath_exp.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vectormath_hyp.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vectormath_lib.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vectormath_trig.h
)

add_library(vectorclass INTERFACE ${SOURCES} ${HEADERS})
target_include_directories(
    vectorclass
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if (MSVC)
    # Enable Multiprocessor compile on target
    # target_compile_options(vectorclass PRIVATE /MP)

    # Provides folder tree in visual studio filters
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include PREFIX "Headers" FILES ${HEADERS})
    #source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Sources" FILES ${SOURCES})
endif()