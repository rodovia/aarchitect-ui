cmake_minimum_required(VERSION 3.19)
project(aar-server LANGUAGES C)

macro(FORCE_OPT _NAME VAL TYPE)
    set(${_NAME} ${VAL} CACHE ${TYPE} "" FORCE)
endmacro()

add_executable(aarserver src/main.c src/protocol.cc src/message.c src/users.cc src/conv.c src/pluginhlp.cc)
target_include_directories(aarserver PUBLIC ../include)
target_link_libraries(aarserver websockets_shared mpack extcaller)
# target_include_directories(aarserver PRIVATE "../libraries/thirdparty/Lua")
