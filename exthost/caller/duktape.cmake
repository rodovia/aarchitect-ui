macro(Download_Library_Url NAME URL)
    FetchContent_Declare(${NAME} URL  ${URL})
    FetchContent_GetProperties(${NAME})
    if(NOT ${NAME}_POPULATED)
        FetchContent_Populate(${NAME})
        # add_subdirectory(${${NAME}_SOURCE_DIR} ${${NAME}_BINARY_DIR})
    endif()
endmacro()

include(FetchContent)

Download_Library_Url(duktape
    "https://duktape.org/duktape-2.7.0.tar.xz")

FetchContent_MakeAvailable(duktape)
message( [TRACE] " =>> duktape_SOURCE_DIR = ${duktape_SOURCE_DIR} ")

file(GLOB_RECURSE duktape_sources "${duktape_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE duktape_headers "${duktape_SOURCE_DIR}/src/*.h")

option(DUK_USE_ASSERTIONS "" OFF)

add_library(duktape ${duktape_sources})
if (DUK_USE_ASSERTIONS)
    target_compile_definitions(duktape PRIVATE DUK_USE_ASSERTIONS)
endif()
target_include_directories(duktape PUBLIC "${duktape_SOURCE_DIR}/src")

