cmake_minimum_required(VERSION 3.19)
project(aar-server LANGUAGES C)

add_subdirectory(../libraries/thirdparty/c-hashmap "hashmap")

macro(FORCE_OPT _NAME VAL TYPE)
    set(${_NAME} ${VAL} CACHE ${TYPE} "" FORCE)
endmacro()

add_executable(aarserver src/main.c src/protocol.c src/message.c src/users.cc src/conv.c)
target_include_directories(aarserver PUBLIC ../include)
target_link_libraries(aarserver websockets_shared cn-cbor hashmap)
# target_include_directories(aarserver PRIVATE "../libraries/thirdparty/Lua")
