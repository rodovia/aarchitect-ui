cmake_minimum_required(VERSION 3.19)
project(scripthost)

set(SCRIPTHOST_SOURCES
    src/main.cc
)

include(CheckIncludeFile)

# set(CMAKE_REQUIRED_DEFINITIONS)
CHECK_INCLUDE_FILE(getopt.h HAS_GETOPT)
if (NOT HAS_GETOPT) # use fallback implementation
    set(SCRIPTHOST_SOURCES ${SCRIPTHOST_SOURCES};src/getopt.c)
endif()

add_executable(scripthost src/main.cc src/getopt.c)
if (HAS_GETOPT) # this check cannot be put in the above 'if' block
                # since it needs the target object
target_compile_definitions(scripthost PRIVATE -DHAS_GETOPT=1)
endif()

add_library(eteller SHARED 
    src/eventteller/callback.cc)

target_include_directories(eteller PUBLIC "../include")
# target_link_libraries(scripthost eteller)
target_include_directories(scripthost PUBLIC "../Libraries/lua/lua-5.4.3/include" "../include")

