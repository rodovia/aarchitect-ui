﻿cmake_policy(SET CMP0091 NEW) # MSVC runtime library flags are selected by an abstraction.

cmake_minimum_required(VERSION 3.19)
project(aarchitectUI)

set(CMAKE_CXX_STANDARD 17)

macro(FORCE_OPT _NAME VAL TYPE)
    set(${_NAME} ${VAL} CACHE ${TYPE} "" FORCE)
endmacro()

option(AARCHITECT_BUILD_SERVER "Makes aarchitect also build the server" YES)
option(AARCHITECT_BUILD_DOCS "Build documentation" NO)
option(AARCHITECT_WITH_DEBUG_INFO "" NO)
option(AARCHITECT_WITH_ASAN "" NO)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")

mark_as_advanced(AARCHITECT_WITH_DEBUG_INFO)

# IMPORTANTE: A configuração precisa ser a primeira, senão o LWS irá compilar com as opções padrão.
option(AARCHITECT_USE_LEAN_AND_MEAN_LWS "Avoid tests and static libraries to be built along with LWS" ON)
if (AARCHITECT_USE_LEAN_AND_MEAN_LWS)
    message("Using lean and mean libwebsockets...")

    FORCE_OPT(LWS_WITH_NETWORK YES BOOL)
    FORCE_OPT(DISABLE_WERROR YES BOOL)
    
    # Básico
    FORCE_OPT(LWS_WITH_MINIMAL_EXAMPLES NO BOOL)
    FORCE_OPT(LWS_WITH_LIBUV NO BOOL)
    FORCE_OPT(LWS_WITH_LIBEVENT NO BOOL) # força o LWS a usar o poll()
    FORCE_OPT(LWS_WITH_STATIC OFF BOOL)
    FORCE_OPT(LWS_WITH_SECURE_STREAMS OFF BOOL)

    # Desabilitar os testes
    FORCE_OPT(LWS_WITHOUT_TEST_SERVER YES BOOL)
    FORCE_OPT(LWS_WITHOUT_TEST_SERVER_EXTPOLL YES BOOL)
    FORCE_OPT(LWS_WITHOUT_TEST_PING YES BOOL)
    FORCE_OPT(LWS_WITHOUT_TESTAPPS YES BOOL)
    FORCE_OPT(LWS_WITHOUT_TEST_CLIENT YES BOOL)

    # habilitar per-message deflate
    if (WIN32)
        FORCE_OPT(LWS_WITH_BUNDLED_ZLIB ON BOOL)
    endif()
    FORCE_OPT(LWS_WITHOUT_EXTENSIONS YES BOOL)
endif()
set(CMAKE_C_STANDARD 11)

set(BUILD_SHARED_LIBS ON)

# ==> DEPENDÊNCIAS <==
FORCE_OPT(BUILD_EXAMPLES OFF BOOL)
FORCE_OPT(BUILD_TESTS OFF BOOL)

# ~~~~~~~
include(exthost/caller/duktape.cmake)
add_subdirectory("libraries/string")
add_subdirectory(libraries/thirdparty/libuv)
add_subdirectory(libraries/thirdparty/libconfig)
add_subdirectory(libraries/thirdparty/libwebsockets)
add_subdirectory(libraries/thirdparty/log.c)
add_subdirectory(libraries/thirdparty/mpack)
add_subdirectory(libraries/thirdparty/fmt)

set(libUI_DIR libraries/thirdparty/libui)
set(SEARCH_LIBUI_DYNAMIC ON)
find_package(libUI)
# ====

set(aarchitect_SOURCES
    src/main.cc
    src/wSettings.c 
    src/settings.c 
    src/connection/mgmt.c
    src/connection/transport.cc
    src/connection/netabstr.c
    src/pluginhlp.cc
    src/ui/chat.c
)
set(aarchitect_LIBS
    string
    websockets_shared
    extcaller
    config
    config++
    logc
    duktape
    uv
    mpack
    fmt
    ${LIBUI_FILES}
)

set(aarchitect_TARGETS
    aarchitect
    exthost
    aarserver
)

if (WIN32)
    list(APPEND aarchitect_SOURCES 
        resource/icon.rc
    )
    if (NOT SEARCH_LIBUI_DYNAMIC)
        list(APPEND aarchitect_LIBS
            d2d1
            dwrite
            comctl32
            uxtheme
            )
    endif()
endif()

# == ALVOS DE COMPILAÇÃO ==
if (AARCHITECT_BUILD_SERVER)
    add_subdirectory(exthost)
    add_subdirectory(server)
endif()

add_executable(aarchitect ${aarchitect_SOURCES})

if (AARCHITECT_BUILD_DOCS)
    include(FindDoxygen)

    doxygen_add_docs(docs   ${CMAKE_CURRENT_LIST_DIR}/src
                            "${CMAKE_CURRENT_LIST_DIR}/include/aarchitect"
                            "${CMAKE_CURRENT_LIST_DIR}/include"
            ALL 
            WORKING_DIRECTORY "docs" 
            COMMENT "Building API documentation")
endif()

target_link_directories(aarchitect PUBLIC ${LIBUI_PATH} PUBLIC ${SQLITE3_DIR})
target_compile_definitions(aarchitect PUBLIC ${LIBUI_DEFINES} 
                            PUBLIC ${SQLITE3_DIR} ${aarchitect_DEFINES})

if (MSVC)
    target_compile_options(aarchitect PRIVATE /utf-8)
endif()

target_link_libraries(aarchitect PRIVATE ${aarchitect_LIBS})
target_include_directories(aarchitect PUBLIC 
                ${LIBUI_PATH} 
                include
                libraries/thirdparty/cn-cbor/include
                libraries/thirdparty/yaml/include
                libraries/thirdparty/libconfig/lib)
