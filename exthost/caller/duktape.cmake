macro(Download_Library_Url NAME URL)
    FetchContent_Declare(${NAME} URL  ${URL})
    FetchContent_GetProperties(${NAME})
    if(NOT ${NAME}_POPULATED)
        FetchContent_Populate(${NAME})
        # add_subdirectory(${${NAME}_SOURCE_DIR} ${${NAME}_BINARY_DIR})
    endif()
endmacro()

include(FetchContent)

option(DUK_SHARED_LIBRARY "Duktape will be compiled as a shared library." NO)
set(DUKTAPE_LIB_TYPE STATIC)
if (DUK_SHARED_LIBRARY)
    set(DUKTAPE_LIB_TYPE SHARED)
endif()

Download_Library_Url(duktape
    "https://duktape.org/duktape-2.7.0.tar.xz")

FetchContent_MakeAvailable(duktape)
message( [TRACE] " =>> duktape_SOURCE_DIR = ${duktape_SOURCE_DIR} ")

file(GLOB_RECURSE duktape_sources "${duktape_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE duktape_headers "${duktape_SOURCE_DIR}/src/*.h")

set(duktape_EXTRAS_DIR "${duktape_SOURCE_DIR}/extras")
option(DUK_USE_ASSERTIONS "" OFF)

add_library(duktape ${DUKTAPE_LIB_TYPE} ${duktape_sources} 
                    ${duktape_EXTRAS_DIR}/module-duktape/duk_module_duktape.c)
    
if (DUK_USE_ASSERTIONS)
    target_compile_definitions(duktape PRIVATE DUK_USE_ASSERTIONS)
endif()

if (DUKTAPE_LIB_TYPE EQUAL SHARED AND WIN32)
    target_compile_definitions(duktape PUBLIC DUK_F_DLL_BUILD=1 PRIVATE DUK_BUILDING_DLL=1)
endif()
target_include_directories(duktape PUBLIC "${duktape_SOURCE_DIR}/src"
                                            "${duktape_EXTRAS_DIR}/module-duktape")

