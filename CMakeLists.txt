cmake_minimum_required(VERSION 3.19)
project(aarchitectUI)

option(AARCHITECT_USE_LEAN_AND_MEAN_LWS "Avoid tests and static libraries to be built along with LWS" ON)
if (AARCHITECT_USE_LEAN_AND_MEAN_LWS)
    
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
    src/blank.cc
    resource/icon.rc 
#   resource/metadata.rc
)
set(aarchitect_LIBS 
    libui
    tabcf
    string
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
add_dependencies(aarchitect string)
target_link_directories(aarchitect PUBLIC ${LIBUI_PATH})
target_compile_definitions(aarchitect PUBLIC ${LIBUI_DEFINES})

if (MSVC)
    target_compile_options(aarchitect PRIVATE /utf-8)
endif()

target_link_libraries(aarchitect PRIVATE ${aarchitect_LIBS})
target_include_directories(aarchitect PUBLIC ${LIBUI_PATH} include libraries/thirdparty/yaml/include)

add_subdirectory(libraries/tabulatedcf/test)
