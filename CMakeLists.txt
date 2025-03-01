cmake_minimum_required(VERSION 3.15)

project("cexception")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/targets")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/targets")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/targets")

function (library_target TYPE)
    string(TOLOWER ${TYPE} TYPE_LOWERCASE)
    set(NAME "cexception-${TYPE_LOWERCASE}")

    add_library(${NAME} ${TYPE} "src/cexception.c")
    set_target_properties(${NAME} PROPERTIES OUTPUT_NAME "cexception")
    target_include_directories(${NAME} PUBLIC "src")
    target_compile_definitions(${NAME} PRIVATE "CEXCEPTION_BUILDING")

    if (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
        target_compile_options(${NAME} PRIVATE "-Wall" "-Wextra" "-pedantic")
    elseif (MSVC)
        target_compile_options(${NAME} PRIVATE "/W3" "/permissive-")
    endif ()

    if (${TYPE} STREQUAL "SHARED")
        target_compile_definitions(${NAME} PUBLIC "CEXCEPTION_SHARED_LIB")
    elseif (WIN32)
        set_target_properties(${NAME} PROPERTIES PREFIX "lib")
    endif ()
endfunction ()

library_target(SHARED)
library_target(STATIC)

if (EXISTS "${CMAKE_SOURCE_DIR}/test")
    add_subdirectory("test")
endif ()
