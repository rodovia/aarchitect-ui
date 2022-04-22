cmake_minimum_required(VERSION 3.19)
project(aar-server LANGUAGES C)

macro(FORCE_OPT _NAME VAL TYPE)
    set(${_NAME} ${VAL} CACHE ${TYPE} "" FORCE)
endmacro()

add_executable(aarserver src/main.c src/protocol.cc src/message.c src/users.cc src/conv.c src/pluginhlp.cc)
target_include_directories(aarserver PUBLIC ../include)
target_link_libraries(aarserver websockets_shared mpack extcaller)

if (MSVC OR "${CMAKE_C_COMPILER_ID}" MATCHES "Clang")
    if ("${CMAKE_BUILD_TYPE}" MATCHES "Rel")
	target_compile_definitions(aarserver PRIVATE _ITERATOR_DEBUG_LEVEL=0)
    endif()
endif()

# target_include_directories(aarserver PRIVATE "../libraries/thirdparty/Lua")
