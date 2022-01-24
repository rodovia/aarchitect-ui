cmake_minimum_required(VERSION 3.19)
project(TabulatedCF VERSION 0.1a LANGUAGES CXX)

option(TABULATEDCF_SHARED "Makes tabcf be compiled as a shared library" YES)
CHECK_INCLUDE_FILE(pthread.h PTHREAD_FOUND)

set(TABULATEDCF_LIBTYPE STATIC)
if (TABULATEDCF_SHARED)
    set(TABULATEDCF_LIBTYPE SHARED)
endif()

add_library(tabcf ${TABULATEDCF_LIBTYPE} 
    src/fileload.cc)

target_include_directories(tabcf PUBLIC "../../include")
if(TABULATEDCF_SHARED)
    target_compile_definitions(tabcf PRIVATE TABCF_EXPORT_SYMBOLS=1 PUBLIC TABCF_IMPORT_SYMBOLS=1)
endif()
if (PTHREAD_FOUND)
    target_compile_definitions(tabcf PRIVATE HAS_POSIX=1)
endif()