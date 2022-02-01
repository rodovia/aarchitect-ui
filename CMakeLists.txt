cmake_minimum_required(VERSION 3.19)
project(aarchitectUI)

set(CMAKE_C_STANDARD 11)

set(BUILD_SHARED_LIBS ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebugDLL")

# ==> DEPENDÊNCIAS <===
set(libUI_DIR libraries/thirdparty/libui)
set(SEARCH_LIBUI_DYNAMIC ON)
find_package(libUI)
add_subdirectory(libraries/tabulatedcf)

set(aarchitect_SOURCES
    src/main.c 
    src/wSettings.c 
    src/settings.c 
    src/string.c
    src/blank.cc
    resource/icon.rc 
#   resource/metadata.rc
)
set(aarchitect_LIBS 
    libui
    tabcf
)

if (WIN32 AND NOT SEARCH_LIBUI_DYNAMIC)
    list(APPEND aarchitect_LIBS
        d2d1
        dwrite
        comctl32
        uxtheme)
endif()

# == ALVOS DE COMPILAÇÃO ==
add_executable(aarchitect ${aarchitect_SOURCES})
target_link_directories(aarchitect PUBLIC ${LIBUI_PATH})
target_compile_definitions(aarchitect PUBLIC ${LIBUI_DEFINES})

if (MSVC)
    target_compile_options(aarchitect PRIVATE /utf-8)
endif()

target_link_libraries(aarchitect PRIVATE ${aarchitect_LIBS})
target_include_directories(aarchitect PUBLIC ${LIBUI_PATH} include libraries/thirdparty/yaml/include)

add_subdirectory(libraries/tabulatedcf/test)
