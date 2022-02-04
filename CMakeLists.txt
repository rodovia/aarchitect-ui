cmake_minimum_required(VERSION 3.19)
project(aarchitectUI)

macro(FORCE_OPT _NAME VAL TYPE)
    set(${_NAME} ${VAL} CACHE ${TYPE} "" FORCE)
endmacro()

# IMPORTANTE: A configuração precisa ser a primeira, senão o LWS irá compilar com as opções padrão.
option(AARCHITECT_USE_LEAN_AND_MEAN_LWS "Avoid tests and static libraries to be built along with LWS" ON)
if (AARCHITECT_USE_LEAN_AND_MEAN_LWS)
    message("Using lean and mean libwebsockets...")

    FORCE_OPT(LWS_WITH_NETWORK YES BOOL)

    # Básico
    FORCE_OPT(LWS_WITH_MINIMAL_EXAMPLES NO BOOL)
    FORCE_OPT(LWS_WITH_LIBUV NO BOOL)
    FORCE_OPT(LWS_WITH_LIBEVENT NO BOOL)
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
        FORCE_OPT(LWS_WITH_BUNDLED_ZLIB OFF BOOL)
        FORCE_OPT(LWS_WITH_ZLIB ON BOOL)
    endif()
    FORCE_OPT(LWS_WITHOUT_EXTENSIONS NO BOOL)
endif()
set(CMAKE_C_STANDARD 11)

set(BUILD_SHARED_LIBS ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebugDLL")

# ==> DEPENDÊNCIAS <===
set(libUI_DIR libraries/thirdparty/libui)
set(SEARCH_LIBUI_DYNAMIC ON)
find_package(libUI)
add_subdirectory("libraries/string")
add_subdirectory(libraries/tabulatedcf)
add_subdirectory(libraries/thirdparty/libwebsockets)

set(aarchitect_SOURCES
    src/main.c 
    src/wSettings.c 
    src/settings.c 
    src/connection/init.c
    src/connection/sendreceive.c
    src/blank.cc
)
set(aarchitect_LIBS 
    libui
    tabcf
    string
    websockets_shared
)

if (WIN32)
    list(APPEND aarchitect_SOURCES 
        resource/icon.rc 
#       resource/metadata.rc
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
add_executable(aarchitect ${aarchitect_SOURCES})
add_dependencies(aarchitect string)
target_link_directories(aarchitect PUBLIC ${LIBUI_PATH})
target_compile_definitions(aarchitect PUBLIC ${LIBUI_DEFINES})

if (MSVC)
    target_compile_options(aarchitect PRIVATE /utf-8)
endif()

target_link_libraries(aarchitect PRIVATE ${aarchitect_LIBS})
target_include_directories(aarchitect PUBLIC ${LIBUI_PATH} include libraries/thirdparty/yaml/include)

add_subdirectory(libraries/tabulatedcf/test)
