cmake_minimum_required(VERSION 3.19)
project(TabulatedCF VERSION 0.1 LANGUAGES CXX)

option(TABULATEDCF_SHARED "Makes tabcf be compiled as a shared library" YES)
find_package(Threads REQUIRED)

set(TABULATEDCF_LIBTYPE STATIC)
if (TABULATEDCF_SHARED)
    set(TABULATEDCF_LIBTYPE SHARED)
endif()

add_library(tabcf ${TABULATEDCF_LIBTYPE} 
    src/error.c
    src/file.c
    src/lexer.c
    src/parser.c
)

target_link_libraries(tabcf PRIVATE Threads::Threads)
if (CMAKE_USE_PTHREADS_INIT)
    target_compile_definitions(tabcf PUBLIC PTHREADS_COMPAT=1)
elseif(NOT CMAKE_USE_PTHREADS_INIT AND CMAKE_USE_WIN32_THREADS_INIT)
    target_compile_definitions(tabcf PUBLIC WIN32_COMPAT=1)
endif()
target_include_directories(tabcf PUBLIC "../../include")
if(TABULATEDCF_SHARED)
    target_compile_definitions(tabcf PRIVATE TABCF_EXPORT_SYMBOLS=1 PUBLIC TABCF_IMPORT_SYMBOLS=1)
endif()
