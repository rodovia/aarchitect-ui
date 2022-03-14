project(exthost-caller)

set(extcaller_SOURCES 
    src/hook.cc
    src/langprocs.cc
)

macro(FORCE_OPT _NAME VAL TYPE)
    set(${_NAME} ${VAL} CACHE ${TYPE} "" FORCE)
endmacro()
FORCE_OPT(DUK_USE_ASSERTIONS ON BOOL)

set(CMAKE_CXX_STANDARD 17)

add_library(extcaller SHARED
    ${extcaller_SOURCES}
)
include(duktape.cmake)

target_include_directories(extcaller PUBLIC ../../include)
target_link_libraries(extcaller synch fmt::fmt duktape)
